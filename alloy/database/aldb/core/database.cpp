//
//  database.cpp
//  alloy
//
//  Created by Alex Lee on 25/07/2017.
//  Copyright © 2017 Alex Lee. All rights reserved.
//

#include "database.hpp"
#include "statement_handle.hpp"
#include "sql_value.hpp"
#include "transaction.hpp"

namespace aldb {

ThreadLocal<std::unordered_map<std::string, RecyclableHandle>> Database::_s_threadedHandle;

Database::Database(const std::string &path, const Configs &default_configs, const DatabaseOpenCallback &on_opened)
    :aldb::CoreBase(CoreType::DATABASE), _pool(HandlePool::get_pool(path, default_configs, on_opened)){};

const std::string &Database::get_path() const { return _pool->path; }

bool Database::is_opened() const { return !_pool->is_drained(); }

void Database::close(std::function<void(void)> on_closed) { _pool->drain(on_closed); }

void Database::set_config(const std::string &name, const Config &config, Configs::Order order) {
    _pool->set_config(name, config, order);
}

void Database::set_config(const std::string &name, const Config &config) { _pool->set_config(name, config); }

RecyclableStatement Database::prepare(const std::string &sql) {
    RecyclableHandle handle = pop_handle();
    return CoreBase::prepare(handle, sql);
}

bool Database::exec(const std::string &sql) { return CoreBase::exec(pop_handle(), sql, {}); }

bool Database::exec(const std::string &sql, const std::list<const SQLValue> &args) {
    return CoreBase::exec(pop_handle(), sql, args);
}

bool Database::exec_transaction(TransactionBlock transaction, TransactionEventBlock event_handle) {
    std::unordered_map<std::string, RecyclableHandle> *threadedHandle = _s_threadedHandle.get();
    if (threadedHandle->find(get_path()) != threadedHandle->end()) {
        return transaction();
    }
    return CoreBase::exec_transaction(transaction, event_handle);
}

bool Database::begin_transaction(const aldb::TransactionMode mode) {
    RecyclableHandle handle = pop_handle();
    if (handle != nullptr && CoreBase::exec(handle, "BEGIN " + transaction_mode(mode))) {
        std::unordered_map<std::string, RecyclableHandle> *threadedHandle = _s_threadedHandle.get();
        threadedHandle->insert({get_path(), handle});
        return true;
    }
    return false;
}

bool Database::commit() {
    RecyclableHandle handle = pop_handle();
    if (handle != nullptr && CoreBase::exec(handle, "COMMIT")) {
        std::unordered_map<std::string, RecyclableHandle> *threadedHandle = _s_threadedHandle.get();
        threadedHandle->erase(get_path());
        return true;
    }
    return false;
}

bool Database::rollback() {
    RecyclableHandle handle = pop_handle();
    bool result             = false;
    if (handle != nullptr) {
        result = CoreBase::exec(handle, "ROLLBACK");
        std::unordered_map<std::string, RecyclableHandle> *threadedHandle = _s_threadedHandle.get();
        threadedHandle->erase(get_path());
    }
    return result;
}

std::shared_ptr<Transaction> Database::getTransaction() {
    RecyclableHandle handle = _pool->flow_out();  // get a new handle for transaction
    if (handle != nullptr) {
        return std::shared_ptr<Transaction>(new Transaction(handle));
    }
    return nullptr;
}

RecyclableHandle Database::pop_handle() {
    std::unordered_map<std::string, RecyclableHandle> *threadedHandle = _s_threadedHandle.get();
    const auto &iter = threadedHandle->find(get_path());
    if (iter == threadedHandle->end()) {
        return _pool->flow_out();
    }
    return iter->second;
}
}