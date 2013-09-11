# S5PV2XX Platform Support
snd-soc-s3c-dma-objs := s3c-dma.o
snd-soc-s3c-idma-objs := s3c-idma.o
snd-soc-s3c-dma-wrapper-objs := s3c-dma-wrapper.o
snd-soc-s3c-i2s-v2-objs := s3c-i2s-v2.o
snd-soc-s5pc1xx-i2s-objs := s5pc1xx-i2s.o
snd-soc-s5p-i2s_sec-objs := s5p-i2s_sec.o

obj-$(CONFIG_SND_S5PV2XX_SOC) += snd-soc-s3c-dma.o
obj-$(CONFIG_SND_S5PC1XX_SOC_I2S) += snd-soc-s3c-idma.o
obj-$(CONFIG_SND_S5PC1XX_SOC_I2S) += snd-soc-s3c-dma-wrapper.o
obj-$(CONFIG_SND_S5PC1XX_SOC_I2S) += snd-soc-s5pc1xx-i2s.o
obj-$(CONFIG_SND_S5PC1XX_SOC_I2S) += snd-soc-s5p-i2s_sec.o
obj-$(CONFIG_SND_S3C_I2SV2_SOC) += snd-soc-s3c-i2s-v2.o

# Codec support
snd-soc-wm8960-objs := ../codecs/wm8960_gzsd.o

obj-$(CONFIG_SND_SOC_WM8960_GZSD210) += snd-soc-wm8960.o

# S5PV2XX Machine Support
snd-soc-gzsd210-wm8960-objs := gzsd210_wm8960.o

obj-$(CONFIG_SND_S5PV2XX_SOC_WM8960) += snd-soc-gzsd210-wm8960.o


