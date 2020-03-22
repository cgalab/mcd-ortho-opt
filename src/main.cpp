#include <iostream>
#include <chrono>
#include <boost/program_options.hpp>
#include "decomp.h"
#include "view.h"

namespace po = boost::program_options;

int main(int argc, char *argv[]) {
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("input", po::value<std::string>(), "input file")
            ("output", po::value<std::string>(), "output file")
            ("ipe-output", po::value<std::string>(), "ipe file")
            ("view", "open OpenGL interface")
            ;

    po::positional_options_description p;
    p.add("input", 1);
    p.add("output", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    if (vm.count("input")) {
        const auto inFile = vm["input"].as<std::string>();
        
        std::string outFilePath = "", ipeFilePath = "";
        if (vm.count("output")) {
            outFilePath = vm["output"].as<std::string>();
        }
        
        if (vm.count("ipe-output")) {
            ipeFilePath = vm["ipe-output"].as<std::string>();
        }
        
        const auto cd = TrapDecomp(inFile, outFilePath, ipeFilePath);

#ifdef ENABLE_VIEW
        bool bEnableView = vm.count("view");
        
        if (bEnableView) {
            QApplication app(argc, argv);
            QCoreApplication::setApplicationName("Convex");
            QCoreApplication::setApplicationVersion(QT_VERSION_STR);
            MainWindow mainWindow;
            mainWindow.addItem(new TrapDecompGraphicsItem(cd));
            mainWindow.show();

            return app.exec();
        }
#endif
    }
    
    return 0;
}
