#/*
# * @file Makefile
# * @author Patrik Lošťák (xlostap00)
# * @brief Makefile for the project
#*/

LOGIN = xlostap00-xcorejs00-xkachyf00

APP_NAME = PetriNetApp

# Directory for build files
BUILD_DIR = build

.PHONY: all run clean pack doxygen

# default target
all:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake ..
	cd $(BUILD_DIR) && $(MAKE)
	cp $(BUILD_DIR)/$(APP_NAME) ./

# Execute the app
run: all
	./$(APP_NAME)

# cleanup
clean:
	rm -rf $(BUILD_DIR)
	rm -f ./$(APP_NAME)
	rm -rf doc/html doc/latex
	rm -f $(LOGIN).zip
	rm -f src/TODO

# Build and pack the project into a zip file
pack: clean
	zip -r Petri_net.zip src/ examples/ doc/ README.md Makefile Doxyfile CMakeLists.txt misc/ LICENSE

# Generate docs
doxygen:
	doxygen Doxyfile