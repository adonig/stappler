# Copyright (c) 2017 Roman Katuntsev <sbkarr@stappler.org>
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

STAPPLER_ROOT = ../../..

LOCAL_OUTDIR := build
LOCAL_OUTPUT_EXECUTABLE := $(LOCAL_OUTDIR)/gui-test

LOCAL_TOOLKIT := material

LOCAL_ROOT = ..

LOCAL_SRCS_DIRS :=  Classes
LOCAL_SRCS_OBJS :=

LOCAL_INCLUDES_DIRS := Classes
LOCAL_INCLUDES_OBJS :=

$(LOCAL_OUTDIR)/common:
	cp -f -r $(STAPPLER_ROOT)/test/resources/common $(LOCAL_OUTDIR)/common

.local_prebuild: $(LOCAL_OUTDIR)/common

include $(STAPPLER_ROOT)/make/local.mk
