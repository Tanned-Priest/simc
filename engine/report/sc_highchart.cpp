#include "simulationcraft.hpp"

#include "sc_highchart.hpp"

namespace
{
static const char* TEXT_OUTLINE      = "-1px -1px 0 #000000, 1px -1px 0 #000000, -1px 1px 0 #000000, 1px 1px 0 #000000";
static const char* TEXT_COLOR        = "#CACACA";
static const char* TEXT_COLOR_ALT    = "black";

static const char* TEXT_MEAN_COLOR   = "#CC8888";
static const char* TEXT_MAX_COLOR    = "#8888CC";

static const char* CHART_BGCOLOR     = "#333333";
static const char* CHART_BGCOLOR_ALT = "white";

}

using namespace highchart;

sc_chart_formatter_t::sc_chart_formatter_t( std::string bg_color, std::string text_color ) :
  chart_formatter_t(),
  _bg_color( bg_color ),
  _text_color( text_color )

{
}

void sc_chart_formatter_t::do_format( chart_t& c )
{
  c.set( "chart.backgroundColor", _bg_color );
  c.set( "chart.style.fontSize", "14px" );

  c.set( "xAxis.lineColor", _text_color );
  c.set( "xAxis.tickColor", _text_color );
  c.set( "xAxis.title.style.color", _text_color );
  c.set( "xAxis.labels.style.color", _text_color );
  c.set( "xAxis.labels.style.fontSize", "14px" );

  c.set( "yAxis.lineColor", _text_color );
  c.set( "yAxis.tickColor", _text_color );
  c.set( "yAxis.title.style.color", _text_color );
  c.set( "yAxis.labels.style.color", _text_color );
  c.set( "yAxis.labels.style.fontSize", "14px" );

  c.set( "title.style.fontSize", "16px" );
  c.set( "title.style.color", _text_color );
  c.set( "tooltip.backgroundColor", "#3F3E38" );
  c.set( "tooltip.style.color", TEXT_COLOR );
}

default_chart_formatter_t::default_chart_formatter_t() :
    sc_chart_formatter_t( CHART_BGCOLOR, TEXT_COLOR )
{

}

void default_chart_formatter_t::do_format( chart_t& c )
{
  sc_chart_formatter_t::do_format( c );

  c.set( "title.style.textShadow", TEXT_OUTLINE );
  c.set( "xAxis.title.style.textShadow", TEXT_OUTLINE );
  c.set( "yAxis.title.style.textShadow", TEXT_OUTLINE );
  c.set( "xAxis.labels.style.textShadow", TEXT_OUTLINE );
  c.set( "yAxis.labels.style.textShadow", TEXT_OUTLINE );
}

alt_chart_formatter_t::alt_chart_formatter_t() :
    sc_chart_formatter_t( CHART_BGCOLOR_ALT, TEXT_COLOR_ALT )
{

}

std::string highchart::build_id( const stats_t* stats, const std::string& suffix )
{
  std::string s;

  s += "actor" + util::to_string( stats -> player -> index );
  s += "_" + stats -> name_str;
  s += suffix;

  return s;
}

std::string highchart::build_id( const player_t* actor, const std::string& suffix )
{
  std::string s = "actor" + util::to_string( actor -> index );
  s += suffix;
  return s;
}

std::string highchart::build_id( const buff_t* buff, const std::string& suffix )
{
  std::string s = "buff_" + buff -> name_str;
  if ( buff -> player )
    s += "_actor" + util::to_string( buff -> player -> index );

  s += suffix;
  return s;
}

// Init default (shared) json structure
chart_t::chart_t( const std::string& id_str, const sim_t* sim ) :
  sc_js_t(), id_str_( id_str ), height_( 250 ), width_( 550 ), sim_( sim )
{
  assert( ! id_str_.empty() );

  // Select which formatter we use for this chart, depending on sim print_styles
  if ( sim -> print_styles == 1 )
  {
    formatter = std::shared_ptr<chart_formatter_t>( new alt_chart_formatter_t() );
  }
  else
  {
    formatter = std::shared_ptr<chart_formatter_t>( new default_chart_formatter_t() );
  }

  // Let the formatter format the chart
  formatter -> do_format( *this );

}

std::string chart_t::to_target_div() const
{
  std::string str_ = "<div id=\"" + id_str_ + "\"";
  str_ += " style=\"min-width: " + util::to_string( width_ ) + "px;";
  if ( height_ > 0 )
    str_ += " height: " + util::to_string( height_ ) + "px;";
  str_ += "margin: 5px;\"></div>\n";

  return str_;
}

std::string chart_t::to_aggregate_string( bool on_click ) const
{
    rapidjson::StringBuffer b;
    rapidjson::Writer< rapidjson::StringBuffer > writer( b );

    js_.Accept( writer );
    std::string javascript = b.GetString();

    std::string str_;
    if ( on_click )
    {
      assert( ! toggle_id_str_.empty() );
      str_ += "$('#" + toggle_id_str_ + "').on('click', function() {\n";
      str_ += "console.log(\"Loading " + id_str_ + ": " + toggle_id_str_ + " ...\" );\n";
    }
    str_ += "$('#" + id_str_ + "').highcharts(";
    str_ += javascript;
    str_ += ");\n";
    if ( on_click )
    {
      str_ += "});\n";
    }

    return str_;
}

std::string chart_t::to_string() const
{
    rapidjson::StringBuffer b;
    rapidjson::Writer< rapidjson::StringBuffer > writer( b );

    js_.Accept( writer );
    std::string javascript = b.GetString();
    javascript.erase(std::remove(javascript.begin(),javascript.end(), '\n'), javascript.end());
    std::string str_ = "<div id=\"" + id_str_ + "\"";
    str_ += " style=\"min-width: " + util::to_string( width_ ) + "px;";
    if ( height_ > 0 )
      str_ += " height: " + util::to_string( height_ ) + "px;";
    str_ += "margin: 5px;\"></div>\n";
    str_ += "<script>\n";
    if ( ! toggle_id_str_.empty() )
    {
      str_ += "jQuery( document ).ready( function( $ ) {\n";
      str_ += "$('#" + toggle_id_str_ + "').on('click', function() {\n";
      str_ += "console.log(\"Loading " + id_str_ + ": " + toggle_id_str_ + " ...\" );\n";
      str_ += "$('#" + id_str_ + "').highcharts(";
      str_ += javascript;
      str_ += ");\n});\n});\n";
    }
    else
    {
      str_ += "jQuery('#" + id_str_ + "').highcharts(";
      str_ += javascript;
      str_ += ");\n";
    }
    str_ += "</script>\n";

    return str_;
}

std::string chart_t::to_json() const
{
  rapidjson::StringBuffer b;
  rapidjson::PrettyWriter< rapidjson::StringBuffer > writer( b );

  js_.Accept( writer );

  return b.GetString();
}

std::string chart_t::to_xml() const
{
  rapidjson::StringBuffer b;
  rapidjson::PrettyWriter< rapidjson::StringBuffer > writer( b );

  js_.Accept( writer );

  std::string str = "<!CDATA[";
  str += b.GetString();
  str += "]]>";

  return str;
}

void chart_t::set_xaxis_max( double max )
{ set( "xAxis.max", max ); }

void chart_t::set_xaxis_title( const std::string& label )
{ set( "xAxis.title.text", label ); }

void chart_t::set_yaxis_title( const std::string& label )
{ set( "yAxis.title.text", label ); }

void chart_t::set_title( const std::string& title )
{ set( "title.text", title ); }

void chart_t::add_data_series( const std::string& type,
                               const std::string& name,
                               const std::vector<data_entry_t>& d )
{

  rapidjson::Value obj( rapidjson::kObjectType );

  if ( ! type.empty() )
    set( obj, "type", type );

  if ( ! name.empty() )
    set( obj, "name", name );

  rapidjson::Value data(rapidjson::kArrayType);

  for ( size_t i = 0; i < d.size(); ++i )
  {
    const data_entry_t& entry = d[ i ];

    rapidjson::Value dataKeys;
    dataKeys.SetObject();

    std::string formatted_name;

    if ( ! entry.color.empty() )
    {
      rapidjson::Value color(entry.color.c_str(), js_.GetAllocator());
      dataKeys.AddMember( "color", js_.GetAllocator(), color, js_.GetAllocator() );

      if ( ! entry.name.empty() )
      {
        formatted_name = "<span style=\"color:" + entry.color;
        formatted_name += "\">" + entry.name + "</span>";
      }
    }
    else if ( ! entry.name.empty() )
      formatted_name = entry.name;

    rapidjson::Value val( entry.value );
    dataKeys.AddMember( "y", js_.GetAllocator(), val, js_.GetAllocator() );

    if ( ! formatted_name.empty() )
    {
      rapidjson::Value name( formatted_name.c_str(), js_.GetAllocator() );
      dataKeys.AddMember( "name", js_.GetAllocator(), name, js_.GetAllocator() );
    }

    data.PushBack(dataKeys, js_.GetAllocator() );
  }

  obj.AddMember("data", js_.GetAllocator(), data, js_.GetAllocator() );
  add("series", obj);
}

void chart_t::add_data_series( const std::vector<data_entry_t>& d )
{ add_data_series( "", "", d ); }

void chart_t::add_simple_series( const std::string& type,
                                 const std::string& color,
                                 const std::string& name,
                                 const std::vector<std::pair<double, double> >& series )
{
  rapidjson::Value obj( rapidjson::kObjectType );

  set( obj, "type", type );
  set( obj, "name", name );

  rapidjson::Value arr( rapidjson::kArrayType );
  for ( size_t i = 0, end = series.size(); i < end; ++i )
  {
    rapidjson::Value pair( rapidjson::kArrayType );
    pair.PushBack( series[ i ].first, js_.GetAllocator() );
    pair.PushBack( series[ i ].second, js_.GetAllocator() );

    arr.PushBack( pair, js_.GetAllocator() );
  }

  set( obj, "data", arr );

  if ( ! color.empty() )
  {
    std::string color_hex = color;
    if ( color_hex[ 0 ] != '#' )
      color_hex = '#' + color_hex;

    add( "colors", color_hex );
  }

  add( "series", obj );
}

void chart_t::add_simple_series( const std::string& type,
                                 const std::string& color,
                                 const std::string& name,
                                 const std::vector<double>& series )
{
  rapidjson::Value obj( rapidjson::kObjectType );

  set( obj, "type", type );
  set( obj, "data", series );
  set( obj, "name", name );

  if ( ! color.empty() )
  {
    std::string color_hex = color;
    if ( color_hex[ 0 ] != '#' )
      color_hex = '#' + color_hex;
    add( "colors", color_hex );
  }

  add( "series", obj );
}

time_series_t::time_series_t( const std::string& id_str, const sim_t* sim ) :
  chart_t( id_str, sim )
{
  set( "legend.enabled", false );

  set( "chart.type", "area" );

  add( "chart.spacing", 5 ).add( "chart.spacing", 5 ).add( "chart.spacing", 5 ).add( "chart.spacing", 5 );

  set_xaxis_title( "Time (seconds)" );

  set( "plotOptions.series.shadow", true );
  set( "plotOptions.area.lineWidth", 1.25 );
  set( "plotOptions.area.states.hover.lineWidth", 1 );
  set( "plotOptions.area.fillOpacity", 0.2 );
}

/**
 * Add y-axis plotline to the chart at value_ height. If name is given, a
 * subtitle will be added to the chart, stating name_=value_. Both the line and
 * the text use color_. line_width_ sets the line width of the plotline. If
 * there are multiple plotlines defined, the subtitle text will be concatenated
 * at the end.
 */
time_series_t& time_series_t::add_yplotline( double value_,
                                             const std::string& name_,
                                             double line_width_,
                                             const std::string& color_ )
{
  if ( rapidjson::Value* obj = path_value( "yAxis.plotLines" ) )
  {
    if ( obj -> GetType() != rapidjson::kArrayType )
      obj -> SetArray();
    set( "credits", false);
    set( "subtitle.style.fontsize", "14px" );
    set( "subtitle.style.textShadow", TEXT_OUTLINE );

    std::string color = color_;
    if ( color.empty() )
      color = "#FFFFFF";

    if ( rapidjson::Value* text_v = path_value( "subtitle.text" ) )
    {
      std::string mean_str = "<span style=\"color: ";
      mean_str += color;
      mean_str += ";\">";
      if ( ! name_.empty() )
        mean_str += name_ + "=";
      mean_str += util::to_string( value_ );
      mean_str += "</span>";

      if ( text_v -> GetType() == rapidjson::kStringType )
        mean_str = std::string( text_v -> GetString() ) + ", " + mean_str;

      text_v -> SetString( mean_str.c_str(), mean_str.size(), js_.GetAllocator() );
    }

    rapidjson::Value new_obj( rapidjson::kObjectType );
    set( new_obj, "color", color );
    set( new_obj, "value", value_ );
    set( new_obj, "width", line_width_ );
    set( new_obj, "zIndex", 5 );

    obj -> PushBack( new_obj, js_.GetAllocator() );
  }

  return *this;
}

time_series_t& time_series_t::set_mean( double value_, const std::string& color )
{
  std::string mean_color = color;
  if ( mean_color.empty() )
    mean_color = TEXT_MEAN_COLOR;

  return add_yplotline( value_, "mean", 1.25, mean_color );
}

time_series_t& time_series_t::set_max( double value_, const std::string& color )
{
  std::string max_color = color;
  if ( max_color.empty() )
    max_color = TEXT_MAX_COLOR;

  return add_yplotline( value_, "max", 1.25, max_color );
}

bar_chart_t::bar_chart_t( const std::string& id_str, const sim_t* sim ) :
    chart_t( id_str, sim )
{
  set( "credits", false);
  set( "legend.enabled", false );

  set( "chart.type", "bar" );

  add( "chart.spacing", 2 ).add( "chart.spacing", 2 ).add( "chart.spacing", 2 ).add( "chart.spacing", 2 );

  set( "plotOptions.series.shadow", true );
  set( "plotOptions.bar.borderWidth", 0 );
  set( "plotOptions.bar.pointWidth", 16 );
  set( "plotOptions.bar.dataLabels.style.color", TEXT_COLOR );
  set( "plotOptions.bar.dataLabels.style.textShadow", TEXT_OUTLINE );
  set( "xAxis.tickLength", 0 );
  set( "xAxis.type", "category" );
  set( "xAxis.labels.style.textShadow", TEXT_OUTLINE );
  set( "xAxis.labels.step", 1 );
  set( "xAxis.labels.y", 4 );
  set( "xAxis.offset", -10 );
}

pie_chart_t::pie_chart_t( const std::string& id_str, const sim_t* sim ) :
    chart_t( id_str, sim )
{
  height_ = 300; // Default Pie Chart height

  set( "credits", false);
  set( "legend.enabled", false );

  set( "chart.type", "pie" );

  add( "chart.spacing", 2 ).add( "chart.spacing", 2 ).add( "chart.spacing", 2 ).add( "chart.spacing", 2 );

  set( "plotOptions.series.shadow", true );
  //set( "plotOptions.bar.states.hover.lineWidth", 1 );
  set( "plotOptions.pie.fillOpacity", 0.2 );
  set( "plotOptions.pie.dataLabels.style.color", TEXT_COLOR );
  set( "plotOptions.pie.dataLabels.style.textShadow", TEXT_OUTLINE );

  set( "plotOptions.pie.dataLabels.enabled", true );
}

histogram_chart_t::histogram_chart_t( const std::string& id_str, const sim_t* sim ) :
    chart_t( id_str, sim )
{
  height_ = 300;

  set( "credits", false);
  set( "legend.enabled", false );
  set( "chart.type", "column" );

  add( "chart.spacing", 5 ).add( "chart.spacing", 5 ).add( "chart.spacing", 5 ).add( "chart.spacing", 5 );

  set( "plotOptions.column.borderWidth", 0 );
  set( "plotOptions.column.pointWidth", 8 );

  set( "xAxis.tickLength", 0 );
  set( "xAxis.type", "category" );
  set( "xAxis.labels.style.textShadow", TEXT_OUTLINE );
}
