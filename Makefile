NAME = draw3D2
SRCS = 3Dmain2.c
OBJS = $(SRCS:.c=.o)

CC = gcc

CCINCPATH = -I/usr/X11R6/include
LDFLAGS = -L/usr/X11R6/lib -lglut -lGL -lm -lGLU

all:$(NAME)

$(NAME):$(OBJS) $(SRCS)
	$(CC) -o $(NAME) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CCINCPATH) $(CCOPTS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(NAME)