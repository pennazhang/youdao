#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "AutoLock.h"

void AutoLock::setAutoLockEnable() {
    [[UIApplication sharedApplication] setIdleTimerDisabled: NO];
}

void AutoLock::setAutoLockDisable() {
    [[UIApplication sharedApplication] setIdleTimerDisabled: YES];
}
