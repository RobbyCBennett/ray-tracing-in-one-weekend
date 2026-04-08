#include "app.hpp"
#include "event.hpp"
#include "random.hpp"
#include "world.hpp"


constexpr S32 SCREEN_WIDTH = 3840;
constexpr S32 SCREEN_HEIGHT = 2160;

constexpr S32 WINDOW_WIDTH = SCREEN_WIDTH / 2;
constexpr S32 WINDOW_HEIGHT = SCREEN_HEIGHT / 2;
constexpr S32 WINDOW_X = SCREEN_WIDTH * 3 / 4 - WINDOW_WIDTH / 2;
constexpr S32 WINDOW_Y = SCREEN_HEIGHT * 1 / 4 - WINDOW_HEIGHT / 2;

constexpr World WORLD;


extern "C" S32 clock();
extern "C" S32 printf(const char* format, ...);
extern "C" S32 puts(const char* str);

static Color ray_color(const Ray& ray, U8 depth);

static void render(Frame& frame);


S32 main()
{
	// Create the main window or fail
	App& app = App::instance();
	WindowId window_id = app.create_main_window(
		"Ray Tracing", FRAMELESS, WINDOW_X, WINDOW_Y, WINDOW_WIDTH, WINDOW_HEIGHT);
	if (not window_id)
		return 1;
	Window window(window_id);

	// Create a frame for CPU-rendering or fail
	Frame frame(WINDOW_WIDTH, WINDOW_HEIGHT);
	if (not frame)
		return 1;

	S32 timestamp = clock();

	render(frame);

	timestamp = clock() - timestamp;
	printf("%d ms\n", timestamp);
	printf("%d fps\n", (S32) (1 / (timestamp / 1000.0)));

	// Main loop
	while (not App::exiting()) {
		Event event = Event::get_blocking();
		if (event.get_kind() == EventKind::KEY_PRESS)
			if (event.get_value().key_press == KEY_ESC)
				App::instance().exit();
		window.draw_frame(frame);
	}
}


Color ray_color(const Ray& ray, U8 depth)
{
	// Base case
	if (depth == 0)
		return Color(0, 0, 0);

	// Spheres
	Hit hit;
	if (WORLD.hit(ray, Interval(0.001, INFINITY), hit)) {
		Ray scattered;
		Color attenuation;
		if (scatter(hit.material, ray, hit, attenuation, scattered))
			return attenuation * ray_color(scattered, depth - 1);
		return Color(0, 0, 0);
	}

	// Sky
	constexpr bool DARK = false;
	constexpr Color SKY_TOP = DARK ? Color(0.0, 0.1, 0.2) : Color(0.5, 0.7, 1);
	constexpr Color SKY_BOTTOM = DARK ? Color(0.2, 0, 0) : Color(1, 1, 1);
	Vec3 unit_direction = ray.direction.unit();
	Float a = (Float) 0.5 * (unit_direction.y + 1);
	return (1 - a) * SKY_BOTTOM + a * SKY_TOP;
}


void render(Frame& frame)
{
	constexpr S32 IMAGE_WIDTH = WINDOW_WIDTH;
	constexpr S32 IMAGE_HEIGHT = WINDOW_HEIGHT;

	constexpr Float VERTICAL_FOV = 20;
	constexpr Vec3 LOOK_FROM {13, 2, 3};
    constexpr Vec3 LOOK_AT {0, 0, 0};
    constexpr Vec3 VERTICAL_UP {0, 1, 0};

	constexpr Float DEFOCUS_ANGLE = 0.6;
	constexpr Float FOCUS_DIST = 10;

	constexpr Vec3 CAMERA_CENTER = LOOK_FROM;

	// Determine viewport dimensions
	constexpr Float THETA = degrees_to_radians(VERTICAL_FOV);
	const Float H = tangent(THETA / 2);
	const Float VIEWPORT_HEIGHT = 2 * H * FOCUS_DIST;
	const Float VIEWPORT_WIDTH = VIEWPORT_HEIGHT * ((Float) IMAGE_WIDTH / IMAGE_HEIGHT);

	// Calculate the u, v, w unit basis vectors for the camera coordinate frame
	const Vec3 W = (LOOK_FROM - LOOK_AT).unit();
	const Vec3 U = (cross(VERTICAL_UP, W)).unit();
	const Vec3 V = cross(W, U);

	// Calculate the vectors across the horizontal and down the vertical viewport edges
	const Vec3 VIEWPORT_U = VIEWPORT_WIDTH * U;
	const Vec3 VIEWPORT_V = VIEWPORT_HEIGHT * -V;

	// Calculate the horizontal and vertial delta vectors from pixel to pixel
	const Vec3 PIXEL_DELTA_U = VIEWPORT_U / IMAGE_WIDTH;
	const Vec3 PIXEL_DELTA_V = VIEWPORT_V / IMAGE_HEIGHT;

	// Calculate the location of the upper left pixel
	const Vec3 VIEWPORT_UPPER_LEFT = CAMERA_CENTER - (FOCUS_DIST * W) - VIEWPORT_U / 2 - VIEWPORT_V / 2;
	const Vec3 PIXEL00_LOCATION = VIEWPORT_UPPER_LEFT + 0.5 * (PIXEL_DELTA_U + PIXEL_DELTA_V);

	// Calculate the camera defocus disk basis vectors
	const Float DEFOCUS_RADIUS = FOCUS_DIST * tangent(degrees_to_radians(DEFOCUS_ANGLE / 2));
	const Vec3 DEFOCUS_DISK_U = U * DEFOCUS_RADIUS;
	const Vec3 DEFOCUS_DISK_V = V * DEFOCUS_RADIUS;

	constexpr bool SAMPLING_AND_BLUR = true;
	constexpr S32 SAMPLES_PER_PIXEL = 10;
	constexpr Float PIXEL_SAMPLES_SCALE = (Float) 1 / SAMPLES_PER_PIXEL;
	constexpr U8 MAX_DEPTH = 10;

	constexpr bool PROGRESS = true;

	Ray ray;
	ray.origin = CAMERA_CENTER;

	// Each row
	for (S32 y = 0; y < WINDOW_HEIGHT; y++) {
		if constexpr (PROGRESS)
			printf("%d / %d rows\r", y, WINDOW_HEIGHT - 1);
		Float y_f = (Float) y;
		Vec3 pixel_y = y_f * PIXEL_DELTA_V;

		// Each pixel on the row
		for (S32 x = 0; x < WINDOW_WIDTH; x++) {
			Float x_f = (Float) x;

			Color color;

			if constexpr (!SAMPLING_AND_BLUR) {
				// Ray direction
				Vec3 pixel_center = PIXEL00_LOCATION + (x_f * PIXEL_DELTA_U) + pixel_y;
				ray.direction = pixel_center - ray.origin;

				color = ray_color(ray, MAX_DEPTH);
			}
			else {
				color = {0, 0, 0};

				for (S32 sample = 0; sample < SAMPLES_PER_PIXEL; sample++) {
					// Ray direction
					Vec3 offset(random() - (Float) 0.5, random() - (Float) 0.5, 0);
					Vec3 pixel_sample = PIXEL00_LOCATION;
					pixel_sample += (x_f + offset.x) * PIXEL_DELTA_U;
					pixel_sample += (y_f + offset.y) * PIXEL_DELTA_V;
					if constexpr (DEFOCUS_ANGLE <= 0) {
						ray.origin = CAMERA_CENTER;
					}
					else {
						Vec3 point = Vec3::random_in_unit_disk();
						ray.origin = CAMERA_CENTER + (point.x * DEFOCUS_DISK_U) + (point.y * DEFOCUS_DISK_V);
					}
					ray.direction = pixel_sample - ray.origin;

					color += ray_color(ray, MAX_DEPTH);
				}

				color *= PIXEL_SAMPLES_SCALE;
			}

			color.x = linear_to_gamma(color.x);
			color.y = linear_to_gamma(color.y);
			color.z = linear_to_gamma(color.z);

			frame.set_pixel(x, y, color.x, color.y, color.z);
		}
	}

	if constexpr (PROGRESS)
		puts("");
}
