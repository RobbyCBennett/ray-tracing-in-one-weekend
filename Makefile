# Variables

PROGRAM := ray_tracing
ifeq ($(OS), Windows_NT)
	PROGRAM := $(PROGRAM).exe
endif

COMPILER := clang++

SOURCE_TYPE := .cpp
HEADER_TYPE := .hpp
OBJECT_TYPE := .o

STANDARD := -std=c++23
OPTIMIZE := -O1

ifeq ($(COMPILER), clang)
	WARNINGS := -Weverything -Wno-covered-switch-default -Wno-implicit-float-conversion -Wno-padded -Wno-reserved-identifier -Wno-switch-default -Wno-unsafe-buffer-usage -Wno-declaration-after-statement
else ifeq ($(COMPILER), clang++)
	WARNINGS := -Weverything -Wno-covered-switch-default -Wno-implicit-float-conversion -Wno-padded -Wno-reserved-identifier -Wno-switch-default -Wno-unsafe-buffer-usage -Wno-c++98-compat-pedantic -Wno-old-style-cast
else ifeq ($(COMPILER), gcc)
	WARNINGS := -Wall -Wno-covered-switch-default -Wno-implicit-float-conversion -Wno-padded -Wno-reserved-identifier -Wno-switch-default -Wno-unsafe-buffer-usage -Wno-declaration-after-statement
else ifeq ($(COMPILER), g++)
	WARNINGS := -Wall -Wno-covered-switch-default -Wno-implicit-float-conversion -Wno-padded -Wno-reserved-identifier -Wno-switch-default -Wno-unsafe-buffer-usage -Wno-c++98-compat-pedantic -Wno-old-style-cast
endif

COMPILE_FLAGS := $(STANDARD) $(OPTIMIZE) $(WARNINGS)

ifeq ($(OS), Windows_NT)
	NO_CONSOLE := -Wl,/subsystem:windows,/entry:mainCRTStartup
	LINK_FLAGS := -ldwmapi -lgdi32 -lglu32 -lopengl32 -luser32 -fuse-ld=lld -Wl,/manifest:embed -Wl,/manifestinput:res/windows.xml
else
	LINK_FLAGS := -lX11
endif

SOURCE_FOLDER := src
OUTPUT_FOLDER := out

ifeq ($(OS), Windows_NT)
	OUTPUT_FOLDER := $(OUTPUT_FOLDER)\windows
else
	OUTPUT_FOLDER := $(OUTPUT_FOLDER)/linux
endif

CLEAN_FOLDERS := $(OUTPUT_FOLDER)

ECHO_NEW_LINE := echo
CLEAN_COMMAND := rm -rf $(CLEAN_FOLDERS)
MKDIR_COMMAND := mkdir -p

ifeq ($(OS), Windows_NT)
	ECHO_NEW_LINE := set _= && echo.
	CLEAN_COMMAND := rmdir /S /Q $(CLEAN_FOLDERS) 1>NUL 2>NUL || set _=
	MKDIR_COMMAND := mkdir
endif

SOURCES := $(wildcard $(SOURCE_FOLDER)/*$(SOURCE_TYPE))
HEADERS := $(wildcard $(SOURCE_FOLDER)/*$(HEADER_TYPE))
OBJECTS := $(patsubst $(SOURCE_FOLDER)/%$(SOURCE_TYPE),$(OUTPUT_FOLDER)/%$(OBJECT_TYPE),$(SOURCES))
PROGRAM := $(OUTPUT_FOLDER)/$(PROGRAM)


# Targets


$(PROGRAM): $(OBJECTS) | $(OUTPUT_FOLDER)
	$(COMPILER) -o $(PROGRAM) $(OBJECTS) $(LINK_FLAGS)

$(OUTPUT_FOLDER)/%$(OBJECT_TYPE): $(SOURCE_FOLDER)/%$(SOURCE_TYPE) $(HEADERS) Makefile | $(OUTPUT_FOLDER)
	$(COMPILER) -o $@ -c $< $(COMPILE_FLAGS)

$(OUTPUT_FOLDER):
	$(MKDIR_COMMAND) $(OUTPUT_FOLDER)


# Commands


clean:
	$(CLEAN_COMMAND)

help:
	@$(ECHO_NEW_LINE)
	@echo make
	@echo     Compile the program
	@$(ECHO_NEW_LINE)
	@echo make clean
	@echo     Remove the obj and bin folders
	@$(ECHO_NEW_LINE)
	@echo make help
	@echo     Get help with the Makefile
	@$(ECHO_NEW_LINE)
	@echo make run
	@echo     Compile the program and run it
	@$(ECHO_NEW_LINE)

run: $(PROGRAM)
	@$(ECHO_NEW_LINE)
	@echo $(PROGRAM)
	@$(ECHO_NEW_LINE)
	@$(PROGRAM)
