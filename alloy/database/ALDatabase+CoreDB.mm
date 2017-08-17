//
//  ALDatabase+CoreDB.m
//  alloy
//
//  Created by Alex Lee on 28/07/2017.
//  Copyright © 2017 Alex Lee. All rights reserved.
//

#import "ALDatabase+CoreDB.h"
#import "__ALDatabase+private.h"
#import "ALUtilitiesHeader.h"
#import "ALLogger.h"

@implementation ALDatabase (CoreDB)

- (BOOL)isOpened {
    return [self _coreDB]->is_opened();
}

- (void)setConfig:(const aldb::Config)config
            named:(NSString *)name
          ordered:(aldb::Configs::Order)order {
    [self _coreDB]->set_config(name.UTF8String, config, order);
}

- (void)setConfig:(const aldb::Config)config named:(NSString *)name {
    [self _coreDB]->set_config(name.UTF8String, config);
}

- (BOOL)exec:(NSString *)sql {
    return [self _coreDB]->exec(sql.UTF8String);
}

- (BOOL)exec:(NSString *)sql args:(const std::list<const ALSQLValue>)args {
    if (args.empty()) {
        return [self exec:sql];
    }
    
    std::list<const aldb::SQLValue> coreValues;
    for (auto arg : args) {
        coreValues.push_back(arg);
    }
    return [self _coreDB]->exec(sql.UTF8String, coreValues);
}

- (BOOL)exec:(NSString *)sql arguments:(NSArray<id> *)args {
    if (args.count == 0) {
        return [self exec:sql];
    }
    
    std::list<const aldb::SQLValue> coreValues;
    for (id arg in args) {
        coreValues.push_back(ALSQLValue(arg));
    }
    return [self _coreDB]->exec(sql.UTF8String, coreValues);
}

- (nullable ALDBResultSet *)query:(NSString *)sql {
    aldb::RecyclableStatement stmt = [self _coreDB]->prepare(sql.UTF8String);
    if (stmt) {
        return [[ALDBResultSet alloc] initWithStatement:stmt];
    }
    ALLogError(@"%s", std::string(*[self _coreDB]->get_error()).c_str());
    return nil;
}

- (nullable ALDBResultSet *)query:(NSString *)sql args:(const std::list<const ALSQLValue>)args {
    aldb::RecyclableStatement stmt = [self _coreDB]->prepare(sql.UTF8String);
    if (stmt) {
        int idx = 1;
        for (auto v : args) {
            stmt->bind_value(v, idx);
            idx++;
        }
        return [[ALDBResultSet alloc] initWithStatement:stmt];
    }
    ALLogError(@"%s", std::string(*[self _coreDB]->get_error()).c_str());
    return nil;
}

- (nullable ALDBResultSet *)query:(NSString *)sql arguments:(NSArray<id> *)args {
    aldb::RecyclableStatement stmt = [self _coreDB]->prepare(sql.UTF8String);
    if (stmt) {
        int idx = 1;
        for (id v in args) {
            stmt->bind_value(ALSQLValue(v), idx);
            idx++;
        }
        return [[ALDBResultSet alloc] initWithStatement:stmt];
    }
    ALLogError(@"%s", std::string(*[self _coreDB]->get_error()).c_str());
    return nil;
}

- (BOOL)inTransaction:(BOOL (^)(void))transactionBlock
         eventHandler:(void (^)(ALDBTransactionEvent event))eventHandler {
    if (transactionBlock == nil) {
        return NO;
    }
    
    aldb::CoreBase::TransactionEventBlock eventBlock = nullptr;
    if (eventHandler) {
        eventBlock = [&eventHandler](aldb::CoreBase::TransactionEvent event) {
            ALSafeInvokeBlock(eventHandler, (ALDBTransactionEvent)event);
        };
    }
    
    return [self _coreDB]->exec_transaction([&transactionBlock]()->bool {
        return transactionBlock();
    },  eventBlock);
}

@end