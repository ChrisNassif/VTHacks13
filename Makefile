SUBDIRS := driver web_controller

.PHONY: all install clean status server open

install:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir install; \
	done

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done

status:
	@$(MAKE) -C game-controller status
	@$(MAKE) -C driver status || true

server:
	@$(MAKE) -C web_controller server

open:
	@$(MAKE) -C web_controller open
