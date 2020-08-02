NAME_PLAY = play
NAME_REC =rec

FLAGS = -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework CoreServices -framework Carbon
LIB_A =  ./libportaudio/lib/.libs/libportaudio.a \
                       ./libsndfile/libsndfile.a \


all: install

rec: $(NAME_PLAY)

$(NAME_PLAY):
	@clang play.c -o play $(FLAGS) $(LIB_A)

install: rec

# -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework CoreServices -framework Carbon ../libportaudio/lib/.libs/libportaudio.a ../libsndfile/libsndfile.a