mkfile_path	:=	$(abspath $(lastword $(MAKEFILE_LIST)))
current_dir	:=	$(BOREALIS_PATH)/$(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

LIBS		:=	-ldeko3dd -lnx -lm $(LIBS)

SOURCES		:=	$(SOURCES) \
				$(current_dir)/lib \
				$(current_dir)/lib/platform_drivers \
				$(current_dir)/extern/glad \
				$(current_dir)/extern/nanovg-deko/source \
				$(current_dir)/extern/nanovg-deko/source/framework \
				$(current_dir)/extern/libretro-common/compat \
				$(current_dir)/extern/libretro-common/encodings \
				$(current_dir)/extern/libretro-common/features \
				$(current_dir)/extern/glad/source

INCLUDES	:=	$(INCLUDES) \
				$(current_dir)/include \
				$(current_dir)/extern/glad \
				$(current_dir)/extern/nanovg-deko/include \
				$(current_dir)/extern/libretro-common/include \
				$(current_dir)/extern/glad/include
