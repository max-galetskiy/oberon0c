//
// Created by M on 13.03.2025.
//

#ifndef OBERON0C_COMMAND_LINE_OPTIONS_HPP
#define OBERON0C_COMMAND_LINE_OPTIONS_HPP

#include <memory>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

std::shared_ptr<po::options_description> init_options();

std::shared_ptr<po::variables_map> create_value_map(po::options_description* options, const int argc, const char** argv);

#endif //OBERON0C_COMMAND_LINE_OPTIONS_HPP
