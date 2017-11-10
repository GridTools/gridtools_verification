/**
 *
 */

#pragma once

#include "../common.h"
#include "../core/command_line.h"
#include "../core/error.h"

namespace gt_verification {

    template < typename Environment >
    int gridtools_verification_main(int argc, char *argv[], std::string data_name, const std::string archive_type="Binary") {
        // Pass command line arguments to googletest
        testing::InitGoogleTest(&argc, argv);

        gt_verification::command_line cl(argc, argv);

        if (cl.has("help"))
            cl.print_help(argv[0]);

        if (cl.has("error") && (cl.as< std::string >("error").find("help") != std::string::npos))
            gt_verification::verification_specification::print_help(argv[0]);

        // Benchmark keywords are meaningless in unittests
        if (cl.has("benchmark"))
            error::fatal("benchmark specification (--benchmark) in unittest executable");

        // Register test environment
        Environment::instance_ = (Environment *)testing::AddGlobalTestEnvironment(new Environment(cl, data_name, archive_type));

        // Run all tests
        int ret = RUN_ALL_TESTS();
        return ret;
    }
}
