SRC := src
BUILD := build
TARGET := twwstuff

QT_SRC_DIR := $(SRC)/Qt
CMD_SRC_DIR := $(SRC)/Cmd
EXCLUDE_QT := -path $(QT_SRC_DIR) -prune -false -o
EXCLUDE_CMD := -path $(CMD_SRC_DIR) -prune -false -o

INCDIRS := -I$(SRC)
LIBS := 

cmd:
	$(MAKE) -f Makefile.cmdline

qt:
	$(MAKE) -f Makefile.Qt moc
	$(MAKE) -f Makefile.Qt all

moc:
	$(MAKE) -f Makefile.Qt moc

moc_clean:
	$(MAKE) -f Makefile.Qt moc_clean

clean:
	$(MAKE) -f Makefile.Qt clean
	$(MAKE) -f Makefile.cmdline clean

.PHONY: cmd qt moc clean_moc clean