# name of the program (Minix service)
PROG=proj

# source code files to be compiled
SRCS = proj.c utils.c timer.c kbc.c keyboard.c mouse.c vbe.c video_gr.c graphics.c cursor.c canvas.c button.c font.c textbox.c dispatcher.c game.c rtc.c clue.c menu.c date.c uart.c protocol.c queue.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
# __LCOM_OPTIMIZED__ disables most of the LCF's overhead (and functionality!)
CFLAGS += -pedantic #-D __LCOM_OPTIMIZED__

# list of library dependencies (for Lab 2, only LCF library)
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
