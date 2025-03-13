//
// Created by M on 13.03.2025.
//

#include "command_line_options.hpp"
#include "panic.h"

std::shared_ptr<po::options_description> init_options(){

    auto options = std::make_shared<po::options_description>("Options");

    options->add_options()
            ("help,h", "Displays this information.")
            ("debug,d","Enables debug prints.")
            ("quiet,q","Disables all outputs.")
            ("filetype,f", po::value<std::string>()->value_name("<type>"),"Sets output filetype. Default is \'.ll\'. [asm,obj,ll]")
            ("print,p","Prints compiled program to console.")
            // custom triplet
            // custom output filename
            // (run?)
            ;

    return options;
}

std::shared_ptr<po::variables_map> create_value_map(po::options_description* options, const int argc, const char** argv){

    auto vm = std::make_shared<po::variables_map>();

    auto input_option = po::options_description("Input");
    input_option.add_options()("input",po::value<std::string>());

    auto all_options = po::options_description();
    all_options.add(*options).add(input_option);

    try{

        po::store(po::command_line_parser(argc,argv)
                .options(all_options)
                .positional(po::positional_options_description().add("input",-1))
                .run(),
        *vm);

        return vm;

    } catch(po::error &e){
        panic(e.what());
    }

};
