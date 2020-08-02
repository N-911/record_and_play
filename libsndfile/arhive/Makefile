NAME = libsndfile.a

INC = sndfile.h

INCI = src/sndfile.h

CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic

SRC = aiff.c \
      alac.c \
      alaw.c \
      au.c \
      audio_detect.c \
      avr.c \
      broadcast.c \
      caf.c \
      cart.c \
      chanmap.c \
      chunk.c \
      command.c \
      common.c \
      dither.c \
      double64.c \
      dwd.c \
      dwvw.c \
      file_io.c \
      flac.c \
      float32.c \
      g72x.c \
      gsm610.c \
      htk.c \
      id3.c \
      ima_adpcm.c \
      ima_oki_adpcm.c \
      interleave.c \
      ircam.c \
      macos.c \
      mat4.c \
      mat5.c \
      mpc2k.c \
      ms_adpcm.c \
      nist.c \
      ogg.c \
      ogg_opus.c \
      ogg_pcm.c \
      ogg_speex.c \
      ogg_vorbis.c \
      paf.c \
      pcm.c \
      pvf.c \
      raw.c \
      rf64.c \
      rx2.c \
      sd2.c \
      sds.c \
      sndfile.c \
      strings.c \
      svx.c \
      test_audio_detect.c \
      test_binheader_writef.c \
      test_broadcast_var.c \
      test_cart_var.c \
      test_conversions.c \
      test_endswap.c \
      test_file_io.c \
      test_float.c \
      test_ima_oki_adpcm.c \
      test_log_printf.c \
      test_main.c \
      test_strncpy_crlf.c \
      txw.c \
      ulaw.c \
      voc.c \
      vox_adpcm.c \
      w64.c \
      wav.c \
      wavlike.c \
      windows.c \
      wve.c \
      xi.c \

SRCS = $(addprefix src/, $(SRC))
OBJ_DIR = obj
OBJ = $(SRC:.c \=.o)

all: install clean
install:
	@cp $(SRCS) .
	@cp $(INCI) .
	@clang $(CFLAGS) -c $(SRC) -I $(INC)
	@ar -rcs $(NAME) $(OBJ)
	@mkdir obj
	@mv $(OBJ) ./obj

uninstall:
	@rm -rf $(NAME)

clean:
	@rm -rf $(INC)
	@rm -rf ./obj
	@rm -rf $(SRC)
	@rm -rf $(OBJ)

reinstall: uninstall all

