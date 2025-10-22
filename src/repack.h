#pragma once

__attribute__((__visibility__("default"))) void
libdng_repack(const uint8_t *src_buf, uint8_t *dst_buf, uint32_t width, uint32_t height, int bitdepth, unsigned int stride);