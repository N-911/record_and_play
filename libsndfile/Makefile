NAME = libsndfile.a

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
      ALAC/ALACBitUtilities.c \
      ALAC/ag_dec.c \
      ALAC/ag_enc.c \
      ALAC/alac_decoder.c \
      ALAC/alac_encoder.c \
      ALAC/dp_dec.c \
      ALAC/dp_enc.c \
      ALAC/matrix_dec.c \
      ALAC/matrix_enc.c \
      G72x/g721.c \
      G72x/g723_16.c \
      G72x/g723_24.c \
      G72x/g723_40.c \
      G72x/g72x.c \
      G72x/g72x_test.c \
      GSM610/add.c \
      GSM610/code.c \
      GSM610/decode.c \
      GSM610/gsm_create.c \
      GSM610/gsm_decode.c \
      GSM610/gsm_destroy.c \
      GSM610/gsm_encode.c \
      GSM610/gsm_option.c \
      GSM610/long_term.c \
      GSM610/lpc.c \
      GSM610/preprocess.c \
      GSM610/rpe.c \
      GSM610/short_term.c \
      GSM610/table.c \

SRCS = $(addprefix src/, $(SRC))
OBJ_DIR = obj
OBJS = $(SRCS:.c=.o)
CFLAGS = -std=c11 -Wall
HEADER = src/sndfile.h

all: install

install: $(NAME)

$(NAME): $(OBJS)
	@ar -rcs $(NAME) $(OBJS)
	@mkdir -p $(OBJ_DIR)
	@mv $(OBJS) $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRCS)
	@clang $(CFLAGS) -o $@ -c $< $(HEADER)

uninstall:
	@rm -rf $(NAME)

clean:
	@rm -rf ./obj

reinstall: uninstall install


