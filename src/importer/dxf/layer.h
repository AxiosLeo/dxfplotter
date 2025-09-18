#pragma once

#include <geometry/polyline.h>

#include <common/aggregable.h>
#include <map>
#include <string>

namespace importer::dxf
{

struct PolylineWithAttributes
{
	geometry::Polyline polyline;
	std::map<std::string, std::string> attributes;
	
	PolylineWithAttributes(const geometry::Polyline& poly) : polyline(poly) {}
	PolylineWithAttributes(const geometry::Polyline& poly, const std::map<std::string, std::string>& attrs) 
		: polyline(poly), attributes(attrs) {}
};

class Layer : public common::Aggregable<Layer>
{
private:
	std::vector<PolylineWithAttributes> m_polylinesWithAttributes;
	std::string m_name;

public:
	Layer() = default;

	explicit Layer(const std::string& name);

	void addPolyline(const geometry::Polyline& polyline);
	void addPolylineWithAttributes(const geometry::Polyline& polyline, const std::map<std::string, std::string>& attributes);

	geometry::Polyline::List polylines() const;
	const std::vector<PolylineWithAttributes>& polylinesWithAttributes() const;

	const std::string& name() const;
};

}

