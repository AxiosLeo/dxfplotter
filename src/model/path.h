#pragma once

#include <geometry/polyline.h>

#include <common/aggregable.h>

#include <model/renderable.h>
#include <model/pathsettings.h>
#include <model/offsettedpath.h>

#include <serializer/access.h>

#include <QTransform>
#include <map>
#include <string>

namespace model
{

class Layer;

class Path : public Renderable, public common::Aggregable<Path>
{
	Q_OBJECT;

	friend serializer::Access<Path>;

private:
	geometry::Polyline m_basePolyline;
	std::unique_ptr<model::OffsettedPath> m_offsettedPath;
	PathSettings m_settings;
	Layer *m_layer;
	bool m_globallyVisible;
	std::map<std::string, std::string> m_attributes;  // Store DXF attributes like "DN600", "钢管" etc.

	void updateGlobalVisibility();

public:
	explicit Path(geometry::Polyline &&basePolyline, const std::string &name, const PathSettings& settings);
	explicit Path() = default;

	static ListUPtr FromPolylines(geometry::Polyline::List &&polylines, const PathSettings &settings, const std::string &layerName);

	Layer &layer();
	const Layer &layer() const;
	void setLayer(Layer &layer);

	const geometry::Polyline &basePolyline() const;
	geometry::Polyline::List finalPolylines() const;

	model::OffsettedPath *offsettedPath() const;
	void offset(float margin, float minimumPolylineLength, float minimumArcLength);
	void resetOffset();
	void pocket(const Path::ListCPtr &islands, float scaledRadius, float minimumPolylineLength, float minimumArcLength);

	void transform(const QTransform &matrix);

	geometry::Rect boundingRect() const;

	bool isPoint() const;

	const PathSettings &settings() const;
	PathSettings &settings();

	// Attribute access methods
	const std::map<std::string, std::string>& attributes() const;
	void setAttribute(const std::string& key, const std::string& value);
	std::string getAttribute(const std::string& key) const;
	bool hasAttribute(const std::string& key) const;

	geometry::CuttingDirection cuttingDirection() const;

	bool globallyVisible() const;

Q_SIGNALS:
	void globalVisibilityChanged(bool globallyVisible);
	void offsettedPathChanged();
	void basePolylineTransformed();
};

}
