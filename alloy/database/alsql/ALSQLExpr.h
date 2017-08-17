//
//  ALSQLExpr.h
//  alloy
//
//  Created by Alex Lee on 03/08/2017.
//  Copyright © 2017 Alex Lee. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <unordered_map>
#import <string>
#import "ALSQLClause.h"
#import "ALDBTypeDefs.h"

class ALDBColumn;
class ALSQLValue;
class ALSQLExpr : public ALSQLClause {
public:
    ALSQLExpr(const ALDBColumn &column);
    ALSQLExpr(const ALSQLValue &value);
    ALSQLExpr(const ALSQLExpr &expr);
    
//    operator ALSQLClause() const;
//    const ALSQLClause &SQLClause() const;
    
    operator bool() const;
    
    static const ALDBOptrPrecedence s_default_precedence;
    static const ALSQLExpr s_null_expr;
    
#pragma mark - sql opetations
    //@link: http://www.sqlite.org/lang_expr.html
    // unary
    ALSQLExpr operator!() const;
    ALSQLExpr operator+() const;
    ALSQLExpr operator-() const;
    ALSQLExpr operator~() const;
    
    // binary
    ALSQLExpr operator||(const ALSQLExpr &r) const;  // or, not concat
    ALSQLExpr operator&&(const ALSQLExpr &r) const;
    ALSQLExpr operator*(const ALSQLExpr &r) const;
    ALSQLExpr operator/(const ALSQLExpr &r) const;
    ALSQLExpr operator%(const ALSQLExpr &r) const;
    ALSQLExpr operator+(const ALSQLExpr &r) const;
    ALSQLExpr operator-(const ALSQLExpr &r) const;
    ALSQLExpr operator<<(const ALSQLExpr &r) const;
    ALSQLExpr operator>>(const ALSQLExpr &r) const;
    ALSQLExpr operator&(const ALSQLExpr &r) const;
    ALSQLExpr operator|(const ALSQLExpr &r) const;
    ALSQLExpr operator<(const ALSQLExpr &r) const;
    ALSQLExpr operator<=(const ALSQLExpr &r) const;
    ALSQLExpr operator>(const ALSQLExpr &r) const;
    ALSQLExpr operator>=(const ALSQLExpr &r) const;
    ALSQLExpr operator==(const ALSQLExpr &r) const;
    ALSQLExpr operator!=(const ALSQLExpr &r) const;
    
    ALSQLExpr in(const std::list<const ALSQLExpr> &expr_list) const;
    ALSQLExpr in(const std::string &table_name) const;
    //    ALSQLExpr in(const SelectStatement &stmt) const;
    ALSQLExpr not_in(const std::list<const ALSQLExpr> &expr_list) const;
    ALSQLExpr not_in(const std::string &table_name) const;
//    ALSQLExpr not_in(const char *table_name) const;
    
    ALSQLExpr like(const ALSQLExpr &expr, const ALSQLExpr &escape = s_null_expr) const;
    ALSQLExpr not_like(const ALSQLExpr &expr, const ALSQLExpr &escape = s_null_expr) const;
    
    ALSQLExpr is_null() const;
    ALSQLExpr not_null() const;
    ALSQLExpr is(const ALSQLExpr &expr) const;
    ALSQLExpr is_not(const ALSQLExpr &expr) const;
    
    static ALSQLExpr cast_as(const ALSQLExpr &expr, const std::string &type_name);
    static ALSQLExpr cast_as(const ALSQLExpr &expr, const ALDBColumnType type);
    //    static ALSQLExpr exists(const SelectStatement &stmt);
    //    static ALSQLExpr not_exists(const SelectStatement &stmt);
    
    // case a when b then c else d end
    static ALSQLExpr case_clause(const ALSQLExpr &expr,
                               const std::list<const std::pair<const ALSQLExpr, const ALSQLExpr>> &when_then,
                               const ALSQLExpr &else_value = s_null_expr);
    static ALSQLExpr case_clause(const std::list<const std::pair<const ALSQLExpr, const ALSQLExpr>> &when_then,
                               const ALSQLExpr &else_value = s_null_expr);
    
    
#pragma mark - sql functions
    //@link: http://www.sqlite.org/lang_corefunc.html
    
    static ALSQLExpr function(const std::string &fun_name,
                              const std::list<const ALSQLExpr> &args,
                              bool distinct = false);
    ALSQLExpr concat(const ALSQLExpr &expr) const; // "abc"||"def"
    ALSQLExpr abs() const;
    ALSQLExpr length() const;
    ALSQLExpr lower() const;
    ALSQLExpr upper() const;
    // expr.instr(str) => "INSTR(str, expr)"
    ALSQLExpr instr(const ALSQLExpr &str) const;
    ALSQLExpr substr(const ALSQLExpr &from) const;
    ALSQLExpr substr(const ALSQLExpr &from, const ALSQLExpr &len) const;
    ALSQLExpr replace(const ALSQLExpr &find, const ALSQLExpr &replacement) const;
    ALSQLExpr ltrim() const;
    ALSQLExpr ltrim(const ALSQLExpr &trim_str) const;
    ALSQLExpr rtrim() const;
    ALSQLExpr rtrim(const ALSQLExpr &trim_str) const;
    ALSQLExpr trim() const;
    ALSQLExpr trim(const ALSQLExpr &trim_str) const;
    ALSQLExpr avg(bool distinct = false) const;
    ALSQLExpr count(bool distinct = false) const;
    ALSQLExpr groupConcat(bool distinct = false) const;
    ALSQLExpr groupConcat(const ALSQLExpr &seperator, bool distinct = false) const;
    ALSQLExpr max(bool distinct = false) const;
    ALSQLExpr min(bool distinct = false) const;
    ALSQLExpr sum(bool distinct = false) const;
    ALSQLExpr total(bool distinct = false) const;
    
#pragma mark -
    static const std::unordered_map<std::string, int> &operator_precedence_map();
    void operate_with(const ALSQLExpr &other,
                      const std::string &optr,
                      ALDBOptrPrecedence optr_precedence = s_default_precedence);
    void enclode_with_brackets();
    
protected:
    static ALDBOptrPrecedence operator_precedence(const std::string &optr);
    
private:
    ALSQLExpr();
    
//    ALSQLClause _clause;
    ALDBOptrPrecedence _precedence;
    
    friend class ALSQLClause;
};
