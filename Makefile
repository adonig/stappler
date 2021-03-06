# Copyright (c) 2016-2017 Roman Katuntsev <sbkarr@stappler.org>
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

.DEFAULT_GOAL := all

STAPPLER_MAKE_FILE := $(realpath $(lastword $(MAKEFILE_LIST)))
STAPPLER_MAKE_ROOT := $(dir $(STAPPLER_MAKE_FILE))
STAPPLER_MAKE_ROOT := $(STAPPLER_MAKE_ROOT:/=)

GLOBAL_ROOT := .

include make/library.mk

all: libmaterial libstappler libcli libcommon

install-deps-ubuntu:
	sudo apt-get install libglew-dev libz-dev libxrandr-dev libxi-dev libxinerama-dev libxcursor-dev

static: libmaterial libstappler

.PHONY: all install-deps-ubuntu
