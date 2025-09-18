#include <iostream>
#include <fstream>
#include <memory>

#include <model/document.h>
#include <model/application.h>
#include <exporter/gcode/exporter.h>
#include <exporter/dxfplot/exporter.h>
#include <importer/dxf/importer.h>

#include <QCoreApplication>
#include <QFileInfo>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <input.dxf> [output_file]" << std::endl;
        std::cout << "  Supported output formats:" << std::endl;
        std::cout << "    .ngc     - G-code format (default)" << std::endl;
        std::cout << "    .dxfplot - JSON format (dxfplot)" << std::endl;
        std::cout << "  Examples:" << std::endl;
        std::cout << "    " << argv[0] << " input.dxf output.ngc" << std::endl;
        std::cout << "    " << argv[0] << " input.dxf output.dxfplot" << std::endl;
        return 1;
    }
    
    const QString inputFile = argv[1];
    QString outputFile;
    
    if (argc > 2) {
        outputFile = argv[2];
    } else {
        // Default to G-code format
        outputFile = inputFile + ".ngc";
    }
    
    // Determine output format based on file extension
    QFileInfo fileInfo(outputFile);
    const QString extension = fileInfo.suffix().toLower();
    const bool isGcode = (extension == "ngc" || extension == "gcode" || extension == "nc") || extension.isEmpty();
    const bool isDxfplot = (extension == "dxfplot" || extension == "json");
    
    try {
        // Create default configuration
        config::Tools::Tool tool("default_tool", YAML::Node());
        config::Profiles::Profile profile("default_profile", YAML::Node());
        
        // Import DXF file with default settings
        importer::dxf::Importer dxfImporter(
            inputFile.toStdString(),
            0.001f,  // splineToArcPrecision
            0.01f,   // minimumSplineLength  
            0.01f    // minimumArcLength
        );
        
        // Create document from layers
        model::Layer::ListUPtr layers;
        
        std::cout << "DEBUG: Processing " << dxfImporter.layers().size() << " layers" << std::endl;
        
        for (auto &importerLayer : dxfImporter.layers()) {
            model::Path::ListUPtr paths;
            
            std::cout << "DEBUG: Layer '" << importerLayer.name() << "' has " 
                      << importerLayer.polylinesWithAttributes().size() << " polylines" << std::endl;
            
            for (const auto &polyWithAttrs : importerLayer.polylinesWithAttributes()) {
                std::cout << "DEBUG: Processing polyline with " << polyWithAttrs.attributes.size() << " attributes" << std::endl;
                
                // Print attributes for debugging
                for (const auto& attr : polyWithAttrs.attributes) {
                    std::cout << "DEBUG: Attribute: " << attr.first << " = " << attr.second << std::endl;
                }
                
                model::PathSettings settings(40, 20, 50, 1); // default settings
                geometry::Polyline polylineCopy = polyWithAttrs.polyline; // Make a copy for move semantics
                auto path = std::make_unique<model::Path>(std::move(polylineCopy), "", settings);
                
                // Apply attributes to the path
                for (const auto& attr : polyWithAttrs.attributes) {
                    std::cout << "DEBUG: Setting attribute " << attr.first << " = " << attr.second << std::endl;
                    path->setAttribute(attr.first, attr.second);
                }
                
                paths.push_back(std::move(path));
            }
            
            auto layer = std::make_unique<model::Layer>(importerLayer.name(), std::move(paths));
            layers.push_back(std::move(layer));
        }
        
        auto taskPtr = std::make_unique<model::Task>(std::move(layers));
        auto document = std::make_unique<model::Document>(std::move(taskPtr), tool, profile);
        
        // Export to the specified format
        std::ofstream output(outputFile.toStdString());
        if (!output.is_open()) {
            std::cerr << "Failed to open output file: " << outputFile.toStdString() << std::endl;
            return 1;
        }
        
        if (isDxfplot) {
            // Export to dxfplot (JSON) format
            exporter::dxfplot::Exporter exporter;
            exporter(*document, output);
            std::cout << "Successfully converted " << inputFile.toStdString() 
                      << " to dxfplot format: " << outputFile.toStdString() << std::endl;
        } else if (isGcode) {
            // Export to G-code format
            const exporter::gcode::Exporter::Options options = static_cast<exporter::gcode::Exporter::Options>(
                exporter::gcode::Exporter::ExportConfig |
                exporter::gcode::Exporter::ExportMetadata
            );
            
            exporter::gcode::Exporter exporter(tool, profile, options);
            exporter(*document, output);
            std::cout << "Successfully converted " << inputFile.toStdString() 
                      << " to G-code format: " << outputFile.toStdString() << std::endl;
        } else {
            std::cerr << "Unsupported output format. Use .ngc/.gcode/.nc for G-code or .dxfplot/.json for dxfplot format." << std::endl;
            return 1;
        }
        
        output.close();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
