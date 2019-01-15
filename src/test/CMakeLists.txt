cmake_minimum_required(VERSION 2.8)

project(token_contract_test)
set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_BUILD_TYPE "Debug")
include_directories(../ )

option(UTIL_TEST  "test " ON)


set(contract_src ../contract/contractobject.cpp
            ../contract/txparse.cpp
            ../contract/designcontract.cpp
            ../contract/contract.cpp)

set(data_src ../data/db_mysql.cpp)
set(contract_machine_src ../contract_machine/contractparse.cpp
            ../contract_machine/actionobject.cpp
            ../contract_machine/machinestack.cpp
            ../contract_machine/assettypes.cpp
            ../contract_machine/action.cpp)

set(internal_src	../internal/handler.cpp
					../internal/handlercli.cpp
					../internal/handlerscan.cpp)

set(transaction_src ../transaction/tx.cpp
                    ../transaction/txobject.cpp)

set(test_src ${transaction_src}
            ${contract_src}
            ${contract_machine_src}
            ${data_src}
            ${internal_src}
			../init.cpp
			../httpserver.cpp
			../parsetx.cpp
			../common/util.cpp
            testingsetup.cpp
            test_contract.cpp
            test_machine.cpp
            test_action.cpp
			test_cli.cpp
            )
add_executable(util_test ${test_src})
target_link_libraries(util_test boost_unit_test_framework c rt  
    curl pthread boost_system boost_program_options event bitcoin 
    mysqlclient glog )


