#
# The author disclaims copyright to this source code.
#
# You can use it any way you want.
#

# TOP-LEVEL SETTINGS

CFG = 		release
VER = 		230beta_09may2014 #consider $VER_STR
HT_ROOT = 	.
WX_ROOT = 	c:/wxWidgets-2.8.12

# RELATIVE LOCATIONS

# -- header files --

HT_INCL =	-I$(HT_ROOT)/incl -I$(HT_ROOT)/incl/domain -I$(HT_ROOT)/incl/dialog
WX_INCL =	-I$(WX_ROOT)/include

# -- source files --

vpath %.rc $(HT_ROOT)/src
vpath %.cpp $(HT_ROOT)/src
vpath %.cpp $(HT_ROOT)/src/domain
vpath %.cpp $(HT_ROOT)/src/dialog

# -- object and library files --

HT_OBJ =	$(HT_ROOT)/obj
WX_LIB = 	-L$(WX_ROOT)/lib/gcc_lib

# OBJECT VARIABLES

# -- HexTools --

COMMON =\
	globaldata \
	hextools \
	hex \
	hextype \
	city \
	application \
	frame \
	hexinfopane \
	mapfeature \
	mapfile \
	mapmemory \
	mapmode \
	mapmouseevent \
	mappane \
	maprenderer \
	maputility \

# -- HexTools-Map --

MAPONLY =\
	terrainselector \
	mapsheet \
	dcity \
	dextendmap \
	mapicons

MAP = $(COMMON) $(MAPONLY)

MAPOBJS = $(patsubst %, $(HT_OBJ)/%.mo, $(MAP))

# -- HexTools-Play --

PLAYONLY =\
	armoreffectsvalue \
	movementcategory \
	counter \
	groundunit \
	groundunittype \
	airunit \
	navalunit \
	hitmarker \
	facility \
	hexnote \
	statusmarker \
	resourcemarker \
	hexcontents \
	groundunitreplpool \
	airunitreplpool \
	hexcontentspane \
	mapairops \
	mapcombat \
	mapconsdemo \
	mapcounter \
	mapmove \
	mapphase \
	mapshowview \
	mapsit \
	mapsupply \
	mapzoc \
	view \
	views \
	daafire \
	dabout \
	daircombatdice \
	dairunit \
	dairunitcombat \
	dairunitoperation \
	dairunitbombingmission \
	dairunitbombingtargets \
	ddisplaysettings \
	dendplayerturn \
	dfacility \
	dgroundcombatdice \
	dgroundunit \
	dhex \
	dhexnote \
	dinsertscenario \
	dnavalunit \
	dnavalunitship \
	dnewgame \
	dstatusmarker \
	dreplacementpool \
	dresourcemarker \
	drules \
	dsearch \
	dsplash \
	dtimejump \
	dunitmover \
	dunitzoc \
	dview \
	dviewcolor \
	dweather \
	playicons

PLAY = $(COMMON) $(PLAYONLY)

PLAYOBJS = $(patsubst %, $(HT_OBJ)/%.po, $(PLAY))

# TOOL OPTIONS

# -- linking --
	
LIBS :=\
		-lcomctl32 \
		-lcomdlg32 \
		-lgdi32 \
		-loleaut32 \
		-lole32 \
		-luuid \
		-lwinspool
		
ifeq ($(CFG),debug)
LIBS :=	$(LIBS) \
		-lwxmsw28d_core \
		-lwxbase28d \
		-lwxpngd \
		-lwxzlibd
else
LIBS :=	$(LIBS) \
		-lwxmsw28_core \
		-lwxbase28 \
		-lwxpng \
		-lwxzlib
endif

LIBSGRP = -Wl,--start-group $(LIBS) -Wl,--end-group

LINKOPTS_DEBUG =\
		-g
	
LINKOPTS_RELEASE =\
		-O2 \
		-static-libgcc \
		-static-libstdc++ \
		-Wall \
		-Wextra

ifeq ($(CFG),debug)
LINKOPTS := $(LINKOPTS_DEBUG) 
else
LINKOPTS := $(LINKOPTS_RELEASE)
endif

LINKOPTS := $(LINKOPTS) -Wl,-subsystem,windows

# -- compilation --

CCOPTS_DEBUG =\
	-g \
	-O0 \
	-D__WXDEBUG__
	
CCOPTS_RELEASE =\
	-O2 \
	-Wall \
	-Wextra \
	-Wpointer-arith \
	-Wcast-align \
	-Wshadow \
	-Wredundant-decls \
	-Wwrite-strings

ifeq ($(CFG),debug)
CCOPTS := $(CCOPTS_DEBUG)
else
CCOPTS := $(CCOPTS_RELEASE)
endif

MAPCCOPTS := -DHEXTOOLSMAP $(CCOPTS)

PLAYCCOPTS := -DHEXTOOLSPLAY $(CCOPTS)

# TARGETS

MAPTGT = 	map$(VER)
PLAYTGT = 	play$(VER)

all: inform map play

inform:
ifneq ($(CFG),release)
ifneq ($(CFG),debug)
	@echo "Invalid configuration "$(CFG)" specified."
	@echo
	@echo "You must specify a configuration when running make, e.g. make CFG=debug"
	@echo  
	@echo "Possible choices for configuration are 'release' and 'debug'."
	@echo
	@exit 1
endif
endif

map: $(MAPTGT)

play: $(PLAYTGT)

clean:
	rm $(HT_OBJ)/*.*o
	rm $(HT_ROOT)/*.exe

# RULES

CC =	g++
RC =	windres

# -- linking --

$(HT_ROOT)/$(MAPTGT): $(MAPOBJS)
	$(CC) $(LINKOPTS) $^ $(WX_LIB) $(LIBSGRP) -o $@

$(HT_ROOT)/$(PLAYTGT): $(PLAYOBJS)
	$(CC) $(LINKOPTS) $^ $(WX_LIB) $(LIBSGRP) -o $@

# -- compilation --

$(HT_OBJ)/%.mo: %.cpp
	$(CC) -c $(MAPCCOPTS) $< $(HT_INCL) $(WX_INCL) -o $@

$(HT_OBJ)/%.po: %.cpp
	$(CC) -c $(PLAYCCOPTS) $< $(HT_INCL) $(WX_INCL) -o $@

$(HT_OBJ)/%.mo: %.rc
	$(RC) --use-temp-file $(WX_INCL) -i $< -o $@

$(HT_OBJ)/%.po: %.rc
	$(RC) --use-temp-file $(WX_INCL) -i $< -o $@
