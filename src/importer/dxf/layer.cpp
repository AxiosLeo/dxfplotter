#include <layer.h>

namespace importer::dxf
{

Layer::Layer(const std::string& name)
	:m_name(name)
{
}

void Layer::addPolyline(const geometry::Polyline& polyline)
{
	m_polylinesWithAttributes.emplace_back(polyline);
}

void Layer::addPolylineWithAttributes(const geometry::Polyline& polyline, const std::map<std::string, std::string>& attributes)
{
	m_polylinesWithAttributes.emplace_back(polyline, attributes);
}

geometry::Polyline::List Layer::polylines() const
{
	geometry::Polyline::List result;
	result.reserve(m_polylinesWithAttributes.size());
	for (const auto& polyWithAttrs : m_polylinesWithAttributes) {
		result.push_back(polyWithAttrs.polyline);
	}
	return result;
}

const std::vector<PolylineWithAttributes>& Layer::polylinesWithAttributes() const
{
	return m_polylinesWithAttributes;
}

const std::string& Layer::name() const
{
	return m_name;
}

}


