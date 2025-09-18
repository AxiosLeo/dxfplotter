#include <importer/dxf/entityimporter.h>
#include <libdxfrw/drw_base.h>

namespace importer::dxf
{

BaseEntityImporter::BaseEntityImporter(Layer& layer, const Settings& settings)
	:m_layer(layer),
	m_settings(settings)
{
}

void BaseEntityImporter::addPolyline(const geometry::Polyline& polyline)
{
	m_layer.addPolyline(polyline);
}

void BaseEntityImporter::addPolylineWithAttributes(const geometry::Polyline& polyline, const std::map<std::string, std::string> &attributes)
{
	m_layer.addPolylineWithAttributes(polyline, attributes);
}

}
