// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/**
Copyright (c) 2016 Roman Katuntsev <sbkarr@stappler.org>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
**/

#import "SPIOSLoaderView.h"

#define IS_WIDESCREEN ( fabs( ( double )[ [ UIScreen mainScreen ] bounds ].size.height - ( double )568 ) < DBL_EPSILON )

static SPLoaderViewController *_loader = nil;

@implementation SPLoaderViewController {
	UIImageView *imageView;
}

+ (void) cancelLoaderLiew {
	if (_loader) {
		[_loader removeFromParentViewController];
	}
}

- (id)init {
	if (self = [super init]) {
		imageView = nil;
	}
	_loader = self;
	return self;
}

- (CGRect) viewFrame {
	CGRect fullScreenRect = [UIScreen mainScreen].bounds;
	
	UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
	
	//implicitly in Portrait orientation.
	if(UIInterfaceOrientationIsLandscape(orientation)){
		CGRect temp = CGRectZero;
		temp.size.width = fullScreenRect.size.height;
		temp.size.height = fullScreenRect.size.width;
		fullScreenRect = temp;
	}
	
	return fullScreenRect;
}

- (void) loadView {
	UIView *view = [[UIView alloc] initWithFrame:[self viewFrame]];
	view.autoresizesSubviews = YES;
	view.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
	view.contentMode = UIViewContentModeScaleAspectFill;
	self.view = view;
}

- (void) viewDidLoad {
	[super viewDidLoad];
	
	UIImage *image = [UIImage imageNamed:[SPLoaderViewController getLaunchImageName]];
	if (image) {
		UIImageView *view = [[UIImageView alloc] initWithFrame:self.view.frame];
		view.image = image;
		view.autoresizesSubviews = YES;
		view.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
		view.contentMode = UIViewContentModeScaleAspectFill;
		
		[self.view addSubview:view];
		imageView = view;
	}
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	return YES;
}

- (NSUInteger) supportedInterfaceOrientations{
	return UIInterfaceOrientationMaskAll;
}

- (void) willRotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration {
	
}
- (void) didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation {
	if (imageView) {
		UIImage *image = [UIImage imageNamed:[SPLoaderViewController getLaunchImageName]];
		imageView.frame = self.view.frame;
		imageView.image = image;
		[imageView setNeedsDisplay];
	}
}
- (BOOL) shouldAutorotate {
	return YES;
}

- (void) removeFromParentViewController {
	UIView *view = self.view;
	[UIView animateWithDuration:0.5f animations:^{
		view.alpha = 0.0f;
	} completion:^(BOOL finished) {
		[super removeFromParentViewController];
	}];
}

- (void) willMoveToParentViewController:(UIViewController *)parent {
	[super willMoveToParentViewController:parent];
	
	CGRect frame = parent.view.frame;
	self.view.frame = frame;
	[parent.view addSubview:self.view];
}

- (void) didMoveToParentViewController:(UIViewController *)parent {
	[super didMoveToParentViewController:parent];
	[self.view removeFromSuperview];
}

+(BOOL)isDeviceiPhone {
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone) {
		return TRUE;
	}
	return FALSE;
}

+(BOOL)isDeviceiPhone4 {
	if ([[UIScreen mainScreen] bounds].size.height==480) return TRUE;
	return FALSE;
}


+(BOOL)isDeviceRetina {
	if ([[UIScreen mainScreen] respondsToSelector:@selector(displayLinkWithTarget:selector:)] && ([UIScreen mainScreen].scale == 2.0)) {
		return TRUE;
	} else {
		return FALSE;
	}
}

+(BOOL)isDeviceiPhone5 {
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone && [[UIScreen mainScreen] bounds].size.height>480) {
		return TRUE;
	}
	return FALSE;
}

+(NSString*)getLaunchImageName {
	NSArray* images= @[@"LaunchImage.png", @"LaunchImage@2x.png",@"LaunchImage-700@2x.png",@"LaunchImage-568h@2x.png",@"LaunchImage-700-568h@2x.png",@"LaunchImage-700-Portrait@2x~ipad.png",@"LaunchImage-Portrait@2x~ipad.png",@"LaunchImage-700-Portrait~ipad.png",@"LaunchImage-Portrait~ipad.png",@"LaunchImage-Landscape@2x~ipad.png",@"LaunchImage-700-Landscape@2x~ipad.png",@"LaunchImage-Landscape~ipad.png",@"LaunchImage-700-Landscape~ipad.png"];
	
	UIImage *splashImage;
	
	if ([self isDeviceiPhone]) {
		if ([self isDeviceiPhone4] && [self isDeviceRetina]) {
			splashImage = [UIImage imageNamed:images[1]];
			if (splashImage.size.width!=0) { return images[1]; } else { return images[2]; }
		} else if ([self isDeviceiPhone5]) {
			splashImage = [UIImage imageNamed:images[1]];
			if (splashImage.size.width!=0) { return images[3]; } else { return images[4]; }
		} else {
			return images[0]; //Non-retina iPhone
		}
	} else if ([UIApplication sharedApplication].statusBarOrientation ==  UIInterfaceOrientationPortrait || [UIApplication sharedApplication].statusBarOrientation ==  UIInterfaceOrientationPortraitUpsideDown)//iPad Portrait {
		if ([self isDeviceRetina]) {
			splashImage = [UIImage imageNamed:images[5]];
			if (splashImage.size.width!=0) { return images[5]; } else { return images[6]; }
		} else {
			splashImage = [UIImage imageNamed:images[7]];
			if (splashImage.size.width!=0) { return images[7]; } else { return images[8]; }
		} else {
			if ([self isDeviceRetina]) {
				splashImage = [UIImage imageNamed:images[9]];
				if (splashImage.size.width!=0) {
					return images[9];
				} else {
					return images[10];
				}
			} else {
				splashImage = [UIImage imageNamed:images[11]];
				if (splashImage.size.width!=0) {
					return images[11];
				} else {
					return images[12];
				}
			}
		}
}

@end
