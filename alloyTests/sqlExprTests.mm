//
//  sqlExprTests.m
//  alloy
//
//  Created by Alex Lee on 18/08/2017.
//  Copyright © 2017 Alex Lee. All rights reserved.
//

#import <XCTest/XCTest.h>
#import "ALSQLExpr.h"
#import "ALSQLValue.h"
#import "ALDBColumn.h"
#import "ALSQLClause.h"
#import <list>
#import <unordered_map>

@interface sqlExprTests : XCTestCase

@end

@implementation sqlExprTests

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)testSQLExpr {
    {
        std::list<const aldb::SQLValue> args = {1, "ab"};
        auto args1 = args;
        XCTAssertEqual(args1, args);
        
        std::list<const ALSQLValue> alArgs = {1, "ab"};
        auto alArgs1 = alArgs;
        XCTAssertEqual(alArgs1, alArgs);

        ALSQLClause clause("a = ? and b = ?", alArgs);
        auto clause1 = clause;
        XCTAssertEqual(clause1.sql_str(), clause.sql_str());
    }
    
    {
        ALSQLClause clause("abc = ?");
        ALSQLClause clause1 = clause.append(" and def = ?");
        XCTAssertEqual(clause1.sql_str(), "abc = ? and def = ?");
    }
    
    {
        ALSQLExpr expr(ALDBColumn("col1"));
        auto expr1 = (expr > 10 || expr <= 0) && (expr == 1 || expr == 2);
        XCTAssertEqual(expr1.sql_str(), "(col1 > ? OR col1 <= ?) AND (col1 = ? OR col1 = ?)");
    }

    {
        ALSQLExpr expr(ALDBColumn("col1"));
        auto expr1 = expr.sum() == ALSQLExpr(ALDBColumn("col2")) + 5;
        XCTAssertEqual(expr1.sql_str(), "SUM(col1) = col2 + ?");
    }
    
    {
        ALSQLExpr expr = ALSQLExpr::case_clause(ALSQLExpr(ALDBColumn("col1")), {{"abc", 1}, {"def", 2}}, 3);
        XCTAssertEqual(expr.sql_str(), "CASE col1 WHEN ? THEN ? WHEN ? THEN ? ELSE ? END");
    }
    
}

//- (void)testPerformanceExample {
//    // This is an example of a performance test case.
//    [self measureBlock:^{
//        // Put the code you want to measure the time of here.
//    }];
//}

@end
