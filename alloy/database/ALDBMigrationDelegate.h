//
//  ALDBMigrationDelegate.h
//  alloy
//
//  Created by Alex Lee on 27/07/2017.
//  Copyright © 2017 Alex Lee. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "aldb.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Delegate protocol for database migration.
 One databse should have only one migration delegate, otherwise, only one of them(anyone in random order) could work.
 */
@protocol ALDBMigrationDelegate <NSObject>

/**
 return YES if the delegate know how to migrate the specified database.
 */
+ (BOOL)canMigrateDatabaseWithPath:(nonnull in NSString *)path;

/**
 The current database user version.
 If the value is changed,  database will be migrated while next opening.
 */
- (NSInteger)currentVersion;

- (BOOL)manualMigration;

- (BOOL)upgradeFromVersion:(NSInteger)oldVersion
                        to:(NSInteger)newVersion
               usingHandle:(const aldb::RecyclableHandle)handle;
- (BOOL)downgradeFromVersion:(NSInteger)oldVersion
                          to:(NSInteger)newVersion
                 usingHandle:(const aldb::RecyclableHandle)handle;

@optional
- (BOOL)setupDatabaseUsingHandle:(const aldb::RecyclableHandle)handle;

@end

NS_ASSUME_NONNULL_END