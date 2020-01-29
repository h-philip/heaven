SFML_INSTALL_DIR = ~/SFML
COMPILE_FLAGS = -I$(SFML_INSTALL_DIR)/include -std=c++11 -D GCC -Wall
# remove the following line for release
COMPILE_FLAGS += -g -D DEBUG \
	-D INFO \
	-D NETWORK_VERBOSE \
	-D PORTAL_DEBUG
LINK_FLAGS = -L$(SFML_INSTALL_DIR)/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network -lsfml-audio

# src files
SRC_CHARACTERS = \
	out/src/Characters/character.o \
	out/src/Characters/player.o \
	out/src/Characters/enemy.o \
	out/src/Characters/vert_movement.o \
	out/src/Characters/shooter.o \
	out/src/Characters/Bullets/bullet.o \
	out/src/Characters/walker.o \
	out/src/Characters/character_factory.o \
	out/src/Characters/character_helper.o
SRC_GROUNDS = \
	out/src/Grounds/ground.o \
	out/src/Grounds/bad_ground.o \
	out/src/Grounds/solid_ground.o \
	out/src/Grounds/portal.o \
	out/src/Grounds/enemy_border.o \
	out/src/Grounds/Buttons/button.o \
	out/src/Grounds/Buttons/network_button.o \
	out/src/Grounds/Buttons/button_factory.o \
	out/src/Grounds/Buttons/button_helper.o \
	out/src/Grounds/Buttons/buyable_button.o \
	out/src/Grounds/Buttons/start_button.o
SRC_EDITOR = \
	out/src/Editor/editor.o
SRC_NETWORK = \
	out/src/Network/network_manager.o \
	out/src/Network/client.o \
	out/src/Network/host.o
SRC_COLLECTABLES = \
	out/src/Collectables/collectable.o \
	out/src/Collectables/coin.o \
	out/src/Collectables/heart.o \
	out/src/Collectables/collectable_factory.o \
	out/src/Collectables/collectable_helper.o
SRC_BUYABLES = \
	out/src/Buyables/buyable.o \
	out/src/Buyables/buyable_factory.o \
	out/src/Buyables/buyable_helper.o \
	out/src/Buyables/jump_boost.o \
	out/src/Buyables/immortality.o
TINYXML2 = \
	out/tinyxml2/tinyxml2.o
SRC = \
	out/main.o \
	out/src/main_helper.o \
	out/src/world.o \
	out/src/hud.o \
	out/src/key_handler.o \
	out/src/log_window.o \
	out/src/help_overlay.o

ALL_O = \
	$(SRC) \
	$(SRC_CHARACTERS) \
	$(SRC_GROUNDS) \
	$(SRC_EDITOR) \
	$(SRC_NETWORK) \
	$(SRC_COLLECTABLES) \
	$(SRC_BUYABLES) \
	$(TINYXML2)

# out o files
OUT_O = \
	out/main.o \
	out/src/*.o \
	out/tinyxml2/*.o \
	out/src/Characters/*.o \
	out/src/Characters/Bullets/*.o \
	out/src/Grounds/*.o \
	out/src/Grounds/Buttons/*.o \
	out/src/Editor/*.o \
	out/src/Network/*.o \
	out/src/Collectables/*.o \
	out/src/Buyables/*.o



all: heaven

# mkdir out
out:
	@echo -n "Creating directories..."
	@mkdir -p out/src/Characters out/src/Characters/Bullets
	@mkdir -p out/src/Grounds out/src/Grounds/Buttons
	@mkdir -p out/src/Editor 
	@mkdir -p out/src/Network
	@mkdir -p out/src/Collectables
	@mkdir -p out/src/Buyables
	@mkdir -p out/tinyxml2
	@echo "Done"

# Compile a file
out/%.o: %.cpp %.h
	@g++ $(COMPILE_FLAGS) -c -o $@ $<
	@echo g++ ... -c -o $@ $<
	@echo relink > out/relink.out

# Compile main.cpp
out/main.o: main.cpp
	@g++ $(COMPILE_FLAGS) -c -o $@ $<
	@echo g++ ... -c -o $@ $<
	@echo relink >> out/relink.out

heaven: out $(ALL_O) heaven.out

# Compile all sources
$(ALL_O):

# Link to exe
heaven.out: out/relink.out
	g++ $(OUT_O) -o heaven.out $(LINK_FLAGS)
	
release: rebuild
	mkdir out/heaven
	cp heaven.out out/heaven/heaven
	cp -r Ressources out/heaven/Ressources
	cd out && zip -r heaven.zip heaven
	cd out && tar -czvf heaven.tar.gz heaven

rebuild: clean heaven

clean:
	@rm -rf out/ heaven.out heaven/
