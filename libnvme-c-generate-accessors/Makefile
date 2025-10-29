.DEFAULT_GOAL := all
BUILD-DIR     := .build

.PHONY: all
all: ${BUILD-DIR}
	meson compile -C ${BUILD-DIR}


${BUILD-DIR}: meson.build
ifneq ("$(wildcard ${BUILD-DIR})","")
	meson configure $@
else
	meson setup $@
endif
	@echo "Configuration located in: $@"
	@echo "-------------------------------------------------------"


.PHONY: clean
clean:
ifneq ("$(wildcard ${BUILD-DIR})","")
	meson compile --clean -C ${BUILD-DIR}
endif


.PHONY: purge
purge:
ifneq ("$(wildcard ${BUILD-DIR})","")
	rm -rf ${BUILD-DIR}
#	meson subprojects purge --confirm
endif


.PHONY: test
test: all
	meson $@ -C ${BUILD-DIR}
