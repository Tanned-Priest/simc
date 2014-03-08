// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.hpp"
#include "sc_report.hpp"

namespace { // UNNAMED NAMESPACE ==========================================

// print_html_contents ======================================================

void print_html_contents( report::sc_html_stream& os, sim_t* sim )
{
  size_t c = 2;     // total number of TOC entries
  if ( sim -> scaling -> has_scale_factors() )
    ++c;

  size_t num_players = sim -> players_by_name.size();
  c += num_players;
  if ( sim -> report_targets )
    c += sim -> targets_by_name.size();

  if ( sim -> report_pets_separately )
  {
    for ( size_t i = 0; i < num_players; i++ )
    {
      for ( size_t j = 0; j < sim -> players_by_name[ i ] -> pet_list.size(); ++j )
      {
        pet_t* pet = sim -> players_by_name[ i ] -> pet_list[ j ];

        if ( pet -> summoned && !pet -> quiet )
          ++c;
      }
    }
  }

  os << "\t\t<div id=\"table-of-contents\" class=\"section grouped-first grouped-last\">\n"
     << "\t\t\t<h2 class=\"toggle\">Table of Contents</h2>\n"
     << "\t\t\t<div class=\"toggle-content hide\">\n";

  // set number of columns
  int n;         // number of columns
  const char* toc_class; // css class
  if ( c < 6 )
  {
    n = 1;
    toc_class = "toc-wide";
  }
  else if ( sim -> report_pets_separately )
  {
    n = 2;
    toc_class = "toc-wide";
  }
  else if ( c < 9 )
  {
    n = 2;
    toc_class = "toc-narrow";
  }
  else
  {
    n = 3;
    toc_class = "toc-narrow";
  }

  int pi = 0;    // player counter
  int ab = 0;    // auras and debuffs link added yet?
  for ( int i = 0; i < n; i++ )
  {
    int cs;    // column size
    if ( i == 0 )
    {
      cs = ( int ) ceil( 1.0 * c / n );
    }
    else if ( i == 1 )
    {
      if ( n == 2 )
      {
        cs = ( int ) ( c - ceil( 1.0 * c / n ) );
      }
      else
      {
        cs = ( int ) ceil( 1.0 * c / n );
      }
    }
    else
    {
      cs = ( int ) ( c - 2 * ceil( 1.0 * c / n ) );
    }

    os << "\t\t\t\t<ul class=\"toc " << toc_class << "\">\n";

    int ci = 1;    // in-column counter
    if ( i == 0 )
    {
      os << "\t\t\t\t\t<li><a href=\"#raid-summary\">Raid Summary</a></li>\n";
      ci++;
      if ( sim -> scaling -> has_scale_factors() )
      {
        os << "\t\t\t\t\t<li><a href=\"#raid-scale-factors\">Scale Factors</a></li>\n";
        ci++;
      }
    }
    while ( ci <= cs )
    {
      if ( pi < static_cast<int>( sim -> players_by_name.size() ) )
      {
        player_t* p = sim -> players_by_name[ pi ];
        std::string html_name = p -> name();
        util::encode_html( html_name );
        os << "\t\t\t\t\t<li><a href=\"#player" << p -> index << "\">" << html_name << "</a>";
        ci++;
        if ( sim -> report_pets_separately )
        {
          os << "\n\t\t\t\t\t\t<ul>\n";
          for ( size_t k = 0; k < sim -> players_by_name[ pi ] -> pet_list.size(); ++k )
          {
            pet_t* pet = sim -> players_by_name[ pi ] -> pet_list[ k ];
            if ( pet -> summoned )
            {
              html_name = pet -> name();
              util::encode_html( html_name );
              os << "\t\t\t\t\t\t\t<li><a href=\"#player" << pet -> index << "\">" << html_name << "</a></li>\n";
              ci++;
            }
          }
          os << "\t\t\t\t\t\t</ul>";
        }
        os << "</li>\n";
        pi++;
      }
      if ( pi == static_cast<int>( sim -> players_by_name.size() ) )
      {
        if ( ab == 0 )
        {
          os << "\t\t\t\t\t<li><a href=\"#auras-buffs\">Auras/Buffs</a></li>\n";
          ab = 1;
        }
        ci++;
        os << "\t\t\t\t\t<li><a href=\"#sim-info\">Simulation Information</a></li>\n";
        ci++;
        if ( sim -> report_raw_abilities )
        {
          os << "\t\t\t\t\t<li><a href=\"#raw-abilities\">Raw Ability Summary</a></li>\n";
          ci++;
        }
      }
      if ( sim -> report_targets && ab > 0 )
      {
        if ( ab == 1 )
        {
          pi = 0;
          ab = 2;
        }
        while ( ci <= cs )
        {
          if ( pi < static_cast<int>( sim -> targets_by_name.size() ) )
          {
            player_t* p = sim -> targets_by_name[ pi ];
            os << "\t\t\t\t\t<li><a href=\"#player" << p -> index << "\">"
               << util::encode_html( p -> name() ) << "</a></li>\n";
          }
          ci++;
          pi++;
        }
      }
    }
    os << "\t\t\t\t</ul>\n";
  }

  os << "\t\t\t\t<div class=\"clear\"></div>\n"
     << "\t\t\t</div>\n\n"
     << "\t\t</div>\n\n";
}

// print_html_sim_summary ===================================================

void print_html_sim_summary( report::sc_html_stream& os, sim_t* sim, sim_t::report_information_t& ri )
{
  os << "\t\t\t\t<div id=\"sim-info\" class=\"section\">\n";

  os << "\t\t\t\t\t<h2 class=\"toggle\">Simulation & Raid Information</h2>\n"
     << "\t\t\t\t\t\t<div class=\"toggle-content hide\">\n";

  os << "\t\t\t\t\t\t<table class=\"sc mt\">\n";

  os << "\t\t\t\t\t\t\t<tr class=\"left\">\n"
     << "\t\t\t\t\t\t\t\t<th>Iterations:</th>\n"
     << "\t\t\t\t\t\t\t\t<td>" << sim -> iterations << "</td>\n"
     << "\t\t\t\t\t\t\t</tr>\n";

  os << "\t\t\t\t\t\t\t<tr class=\"left\">\n"
     << "\t\t\t\t\t\t\t\t<th>Threads:</th>\n"
     << "\t\t\t\t\t\t\t\t<td>" << ( ( sim -> threads < 1 ) ? 1 : sim -> threads ) << "</td>\n"
     << "\t\t\t\t\t\t\t</tr>\n";

  os << "\t\t\t\t\t\t\t<tr class=\"left\">\n"
     << "\t\t\t\t\t\t\t\t<th>Confidence:</th>\n"
     << "\t\t\t\t\t\t\t\t<td>" << sim -> confidence * 100.0 << "%</td>\n"
     << "\t\t\t\t\t\t\t</tr>\n";

  os.printf( "\t\t\t\t\t\t\t<tr class=\"left\">\n"
             "\t\t\t\t\t\t\t\t<th>Fight Length%s:</th>\n"
             "\t\t\t\t\t\t\t\t<td>%.0f - %.0f ( %.1f )</td>\n"
             "\t\t\t\t\t\t\t</tr>\n",
             (sim -> fixed_time ? " (fixed time)" : ""),
             sim -> simulation_length.min(),
             sim -> simulation_length.max(),
             sim -> simulation_length.mean() );

  os << "\t\t\t\t\t\t\t<tr class=\"left\">\n"
     << "\t\t\t\t\t\t\t\t<th><h2>Performance:</h2></th>\n"
     << "\t\t\t\t\t\t\t\t<td></td>\n"
     << "\t\t\t\t\t\t\t</tr>\n";

  os.printf(
    "\t\t\t\t\t\t\t<tr class=\"left\">\n"
    "\t\t\t\t\t\t\t\t<th>Total Events Processed:</th>\n"
    "\t\t\t\t\t\t\t\t<td>%ld</td>\n"
    "\t\t\t\t\t\t\t</tr>\n",
    ( long ) sim -> total_events_processed );

  os.printf(
    "\t\t\t\t\t\t\t<tr class=\"left\">\n"
    "\t\t\t\t\t\t\t\t<th>Max Event Queue:</th>\n"
    "\t\t\t\t\t\t\t\t<td>%ld</td>\n"
    "\t\t\t\t\t\t\t</tr>\n",
    ( long ) sim -> max_events_remaining );

  os.printf(
    "\t\t\t\t\t\t\t<tr class=\"left\">\n"
    "\t\t\t\t\t\t\t\t<th>Sim Seconds:</th>\n"
    "\t\t\t\t\t\t\t\t<td>%.0f</td>\n"
    "\t\t\t\t\t\t\t</tr>\n",
    sim -> iterations * sim -> simulation_length.mean() );
  os.printf(
    "\t\t\t\t\t\t\t<tr class=\"left\">\n"
    "\t\t\t\t\t\t\t\t<th>CPU Seconds:</th>\n"
    "\t\t\t\t\t\t\t\t<td>%.4f</td>\n"
    "\t\t\t\t\t\t\t</tr>\n",
    sim -> elapsed_cpu.total_seconds() );
  os.printf(
    "\t\t\t\t\t\t\t<tr class=\"left\">\n"
    "\t\t\t\t\t\t\t\t<th>Physical Seconds:</th>\n"
    "\t\t\t\t\t\t\t\t<td>%.4f</td>\n"
    "\t\t\t\t\t\t\t</tr>\n",
    sim -> elapsed_time.total_seconds() );
  os.printf(
    "\t\t\t\t\t\t\t<tr class=\"left\">\n"
    "\t\t\t\t\t\t\t\t<th>Speed Up:</th>\n"
    "\t\t\t\t\t\t\t\t<td>%.0f</td>\n"
    "\t\t\t\t\t\t\t</tr>\n",
    sim -> iterations * sim -> simulation_length.mean() / sim -> elapsed_cpu.total_seconds() );

  os << "\t\t\t\t\t\t\t<tr class=\"left\">\n"
     << "\t\t\t\t\t\t\t\t<th><h2>Settings:</h2></th>\n"
     << "\t\t\t\t\t\t\t\t<td></td>\n"
     << "\t\t\t\t\t\t\t</tr>\n";

  os.printf(
    "\t\t\t\t\t\t\t<tr class=\"left\">\n"
    "\t\t\t\t\t\t\t\t<th>World Lag:</th>\n"
    "\t\t\t\t\t\t\t\t<td>%.0f ms ( stddev = %.0f ms )</td>\n"
    "\t\t\t\t\t\t\t</tr>\n",
    ( double )sim -> world_lag.total_millis(), ( double )sim -> world_lag_stddev.total_millis() );
  os.printf(
    "\t\t\t\t\t\t\t<tr class=\"left\">\n"
    "\t\t\t\t\t\t\t\t<th>Queue Lag:</th>\n"
    "\t\t\t\t\t\t\t\t<td>%.0f ms ( stddev = %.0f ms )</td>\n"
    "\t\t\t\t\t\t\t</tr>\n",
    ( double )sim -> queue_lag.total_millis(), ( double )sim -> queue_lag_stddev.total_millis() );

  if ( sim -> strict_gcd_queue )
  {
    os.printf(
      "\t\t\t\t\t\t\t<tr class=\"left\">\n"
      "\t\t\t\t\t\t\t\t<th>GCD Lag:</th>\n"
      "\t\t\t\t\t\t\t\t<td>%.0f ms ( stddev = %.0f ms )</td>\n"
      "\t\t\t\t\t\t\t</tr>\n",
      ( double )sim -> gcd_lag.total_millis(), ( double )sim -> gcd_lag_stddev.total_millis() );
    os.printf(
      "\t\t\t\t\t\t\t<tr class=\"left\">\n"
      "\t\t\t\t\t\t\t\t<th>Channel Lag:</th>\n"
      "\t\t\t\t\t\t\t\t<td>%.0f ms ( stddev = %.0f ms )</td>\n"
      "\t\t\t\t\t\t\t</tr>\n",
      ( double )sim -> channel_lag.total_millis(), ( double )sim -> channel_lag_stddev.total_millis() );
    os.printf(
      "\t\t\t\t\t\t\t<tr class=\"left\">\n"
      "\t\t\t\t\t\t\t\t<th>Queue GCD Reduction:</th>\n"
      "\t\t\t\t\t\t\t\t<td>%.0f ms</td>\n"
      "\t\t\t\t\t\t\t</tr>\n",
      ( double )sim -> queue_gcd_reduction.total_millis() );
  }

  int sd_counter = 0;
  report::print_html_sample_data( os, sim, sim -> simulation_length, "Simulation Length", sd_counter );

  os << "\t\t\t\t\t\t</table>\n";

  // Left side charts: dps, gear, timeline, raid events

  os << "\t\t\t\t<div class=\"charts charts-left\">\n";
  // Timeline Distribution Chart
  if ( sim -> iterations > 1 && ! ri.timeline_chart.empty() )
  {
    os.printf(
      "\t\t\t\t\t<a href=\"#help-timeline-distribution\" class=\"help\"><img src=\"%s\" alt=\"Timeline Distribution Chart\" /></a>\n",
      ri.timeline_chart.c_str() );
  }

  // Gear Charts
  for ( size_t i = 0; i < ri.gear_charts.size(); i++ )
  {
    os << "\t\t\t\t\t<img src=\"" << ri.gear_charts[ i ] << "\" alt=\"Gear Chart\" />\n";
  }

  // Raid Downtime Chart
  if ( !  ri.downtime_chart.empty() )
  {
    os.printf(
      "\t\t\t\t\t<img src=\"%s\" alt=\"Raid Downtime Chart\" />\n",
      ri.downtime_chart.c_str() );
  }

  os << "\t\t\t\t</div>\n";

  // Right side charts: dpet
  os << "\t\t\t\t<div class=\"charts\">\n";

  for ( size_t i = 0; i < ri.dpet_charts.size(); i++ )
  {
    os.printf(
      "\t\t\t\t\t<img src=\"%s\" alt=\"DPET Chart\" />\n",
      ri.dpet_charts[ i ].c_str() );
  }

  os << "\t\t\t\t</div>\n";


  // closure
  os << "\t\t\t\t<div class=\"clear\"></div>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n\n";
}


// print_html_raw_ability_summary ===========================================

double aggregate_damage( const std::vector<stats_t::stats_results_t>& result )
{
  double total = 0;
  for ( size_t i = 0; i < result.size(); i++ )
  {
    total  += result[ i ].fight_actual_amount.mean();
  }
  return total;
}

int find_id( stats_t* s )
{
  int id = 0;
  /*
  for ( size_t i = 0; i < s -> player -> action_list.size(); ++i )
    {
      action_t* a = s -> player -> action_list[ i ];
      if ( a -> stats != s ) continue;
      id = a -> id;
      if ( ! a -> background ) break;
    }
  */
  for ( size_t i = 0; i < s -> action_list.size(); i++ )
  {
    if ( s -> action_list[ i ] -> id != 0 )
    {
      id = s -> action_list[ i ] -> id;
      break;
    }
  }
  return id;
}

void print_html_raw_action_damage( report::sc_html_stream& os, stats_t* s, player_t* p, int j, sim_t* sim )
{
  if ( s -> num_executes.mean() == 0 && s -> compound_amount == 0 && !sim -> debug )
    return;

  os << "\t\t\t<tr";
  if ( j & 1 )
    os << " class=\"odd\"";
  os << ">\n";

  int id = find_id( s );

  char format[] =
    "\t\t\t\t\t<td class=\"left  small\">%s</td>\n"
    "\t\t\t\t\t<td class=\"left  small\">%s</td>\n"
    "\t\t\t\t\t<td class=\"left  small\">%s%s</td>\n"
    "\t\t\t\t\t<td class=\"right small\">%d</td>\n"
    "\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
    "\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
    "\t\t\t\t\t<td class=\"right small\">%.2f</td>\n"
    "\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
    "\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
    "\t\t\t\t\t<td class=\"right small\">%.1f</td>\n"
    "\t\t\t\t\t<td class=\"right small\">%.1f</td>\n"
    "\t\t\t\t\t<td class=\"right small\">%.1f%%</td>\n"
    "\t\t\t\t\t<td class=\"right small\">%.1f%%</td>\n"
    "\t\t\t\t\t<td class=\"right small\">%.1f%%</td>\n"
    "\t\t\t\t\t<td class=\"right small\">%.1f%%</td>\n"
    "\t\t\t\t\t<td class=\"right small\">%.2fsec</td>\n"
    "\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
    "\t\t\t\t\t<td class=\"right small\">%.2fsec</td>\n"
    "\t\t\t\t</tr>\n";

  double direct_total = aggregate_damage( s -> direct_results );
  double tick_total = aggregate_damage( s -> tick_results );
  if ( direct_total > 0.0 || tick_total <= 0.0 )
    os.printf(
      format,
      util::encode_html( p -> name() ).c_str(),
      util::encode_html( s -> player -> name() ).c_str(),
      s -> name_str.c_str(), "",
      id,
      direct_total,
      direct_total / s -> player -> collected_data.fight_length.mean(),
      s -> num_direct_results.mean() / ( s -> player -> collected_data.fight_length.mean() / 60.0 ),
      s -> direct_results[ RESULT_HIT  ].actual_amount.mean(),
      s -> direct_results[ RESULT_CRIT ].actual_amount.mean(),
      s -> num_executes.mean(),
      s -> num_direct_results.mean(),
      s -> direct_results[ RESULT_CRIT ].pct,
      s -> direct_results[ RESULT_MISS ].pct + s -> direct_results[ RESULT_DODGE  ].pct + s -> direct_results[ RESULT_PARRY  ].pct,
      s -> direct_results[ RESULT_GLANCE ].pct,
      s -> direct_results_detail[ FULLTYPE_HIT_BLOCK ].pct + s -> direct_results_detail[ FULLTYPE_HIT_CRITBLOCK ].pct +
      s -> direct_results_detail[ FULLTYPE_GLANCE_BLOCK ].pct + s -> direct_results_detail[ FULLTYPE_GLANCE_CRITBLOCK ].pct +
      s -> direct_results_detail[ FULLTYPE_CRIT_BLOCK ].pct + s -> direct_results_detail[ FULLTYPE_CRIT_CRITBLOCK ].pct,
      s -> total_intervals.mean(),
      s -> total_amount.mean(),
      s -> player -> collected_data.fight_length.mean() );

  if ( tick_total > 0.0 )
    os.printf(
      format,
      util::encode_html( p -> name() ).c_str(),
      util::encode_html( s -> player -> name() ).c_str(),
      s -> name_str.c_str(), " ticks",
      -id,
      tick_total,
      tick_total / sim -> max_time.total_seconds(),
      s -> num_ticks.mean() / sim -> max_time.total_minutes(),
      s -> tick_results[ RESULT_HIT  ].actual_amount.mean(),
      s -> tick_results[ RESULT_CRIT ].actual_amount.mean(),
      s -> num_executes.mean(),
      s -> num_ticks.mean(),
      s -> tick_results[ RESULT_CRIT ].pct,
      s -> tick_results[ RESULT_MISS ].pct + s -> tick_results[ RESULT_DODGE  ].pct + s -> tick_results[ RESULT_PARRY  ].pct,
      s -> tick_results[ RESULT_GLANCE ].pct,
      s -> tick_results_detail[ FULLTYPE_HIT_BLOCK ].pct + s -> tick_results_detail[ FULLTYPE_HIT_CRITBLOCK ].pct +
      s -> tick_results_detail[ FULLTYPE_GLANCE_BLOCK ].pct + s -> tick_results_detail[ FULLTYPE_GLANCE_CRITBLOCK ].pct +
      s -> tick_results_detail[ FULLTYPE_CRIT_BLOCK ].pct + s -> tick_results_detail[ FULLTYPE_CRIT_CRITBLOCK ].pct,
      s -> total_intervals.mean(),
      s -> total_amount.mean(),
      s -> player -> collected_data.fight_length.mean() );

  for ( size_t i = 0, num_children = s -> children.size(); i < num_children; i++ )
  {
    print_html_raw_action_damage( os, s -> children[ i ], p, j, sim );
  }
}

void print_html_raw_ability_summary( report::sc_html_stream& os, sim_t* sim )
{
  os << "\t\t<div id=\"raw-abilities\" class=\"section\">\n\n";
  os << "\t\t\t<h2 class=\"toggle\">Raw Ability Summary</h2>\n"
     << "\t\t\t<div class=\"toggle-content hide\">\n";

  // Abilities Section
  os << "\t\t\t<table class=\"sc\">\n"
     << "\t\t\t\t<tr>\n"
     << "\t\t\t\t\t<th class=\"left small\">Character</th>\n"
     << "\t\t\t\t\t<th class=\"left small\">Unit</th>\n"
     << "\t\t\t\t\t<th class=\"small\"><a href=\"#help-ability\" class=\"help\">Ability</a></th>\n"
     << "\t\t\t\t\t<th class=\"small\"><a href=\"#help-id\" class=\"help\">Id</a></th>\n"
     << "\t\t\t\t\t<th class=\"small\"><a href=\"#help-total\" class=\"help\">Total</a></th>\n"
     << "\t\t\t\t\t<th class=\"small\"><a href=\"#help-dps\" class=\"help\">DPS</a></th>\n"
     << "\t\t\t\t\t<th class=\"small\"><a href=\"#help-ipm\" class=\"help\">Imp/Min</a></th>\n"
     << "\t\t\t\t\t<th class=\"small\"><a href=\"#help-hit\" class=\"help\">Hit</a></th>\n"
     << "\t\t\t\t\t<th class=\"small\"><a href=\"#help-crit\" class=\"help\">Crit</a></th>\n"
     << "\t\t\t\t\t<th class=\"small\"><a href=\"#help-count\" class=\"help\">Count</a></th>\n"
     << "\t\t\t\t\t<th class=\"small\"><a href=\"#help-direct-results\" class=\"help\">Impacts</a></th>\n"
     << "\t\t\t\t\t<th class=\"small\"><a href=\"#help-crit-pct\" class=\"help\">Crit%</a></th>\n"
     << "\t\t\t\t\t<th class=\"small\"><a href=\"#help-miss-pct\" class=\"help\">Avoid%</a></th>\n"
     << "\t\t\t\t\t<th class=\"small\"><a href=\"#help-glance-pct\" class=\"help\">G%</a></th>\n"
     << "\t\t\t\t\t<th class=\"small\"><a href=\"#help-block-pct\" class=\"help\">B%</a></th>\n"
     << "\t\t\t\t\t<th class=\"small\"><a href=\"#help-interval\" class=\"help\">Interval</a></th>\n"
     << "\t\t\t\t\t<th class=\"small\"><a href=\"#help-combined\" class=\"help\">Combined</a></th>\n"
     << "\t\t\t\t\t<th class=\"small\"><a href=\"#help-duration\" class=\"help\">Duration</a></th>\n"
     << "\t\t\t\t</tr>\n";

  int count = 0;
  for ( size_t player_i = 0; player_i < sim -> players_by_name.size(); player_i++ )
  {
    player_t* p = sim -> players_by_name[ player_i ];
    for ( size_t i = 0; i < p -> stats_list.size(); ++i )
    {
      stats_t* s = p -> stats_list[ i ];
      if ( s -> parent == NULL )
        print_html_raw_action_damage( os, s, p, count++, sim );
    }

    for ( size_t pet_i = 0; pet_i < p -> pet_list.size(); ++pet_i )
    {
      pet_t* pet = p -> pet_list[ pet_i ];
      for ( size_t i = 0; i < pet -> stats_list.size(); ++i )
      {
        stats_t* s = pet -> stats_list[ i ];
        if ( s -> parent == NULL )
          print_html_raw_action_damage( os, s, p, count++, sim );
      }
    }
  }

  // closure
  os << "\t\t\t\t</table>\n";
  os << "\t\t\t\t<div class=\"clear\"></div>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n\n";
}

// print_html_raid_summary ==================================================

void print_html_raid_summary( report::sc_html_stream& os, sim_t* sim, sim_t::report_information_t& ri )
{
  os << "\t\t<div id=\"raid-summary\" class=\"section section-open\">\n\n";

  os << "\t\t\t<h2 class=\"toggle open\">Raid Summary</h2>\n";

  os << "\t\t\t<div class=\"toggle-content\">\n";

  os << "\t\t\t<ul class=\"params\">\n";

  os.printf(
    "\t\t\t\t<li><b>Raid Damage:</b> %.0f</li>\n",
    sim -> total_dmg.mean() );
  os.printf(
    "\t\t\t\t<li><b>Raid DPS:</b> %.0f</li>\n",
    sim -> raid_dps.mean() );
  if ( sim -> total_heal.mean() > 0 )
  {
    os.printf(
      "\t\t\t\t<li><b>Raid Heal+Absorb:</b> %.0f</li>\n",
      sim -> total_heal.mean() + sim -> total_absorb.mean() );
    os.printf(
      "\t\t\t\t<li><b>Raid HPS+APS:</b> %.0f</li>\n",
      sim -> raid_hps.mean() + sim -> raid_aps.mean() );
  }
  os << "\t\t\t</ul><p>&nbsp;</p>\n";

  // Left side charts: dps, raid events
  os << "\t\t\t\t<div class=\"charts charts-left\">\n";

  for ( size_t i = 0; i < ri.dps_charts.size(); i++ )
  {
    os.printf(
      "\t\t\t\t\t<map id='DPSMAP%d' name='DPSMAP%d'></map>\n", ( int )i, ( int )i );
    os.printf(
      "\t\t\t\t\t<img id='DPSIMG%d' src=\"%s\" alt=\"DPS Chart\" />\n",
      ( int )i, ri.dps_charts[ i ].c_str() );
  }

  if ( ! sim -> raid_events_str.empty() )
  {
    os << "\t\t\t\t\t<table>\n"
       << "\t\t\t\t\t\t<tr>\n"
       << "\t\t\t\t\t\t\t<th></th>\n"
       << "\t\t\t\t\t\t\t<th class=\"left\">Raid Event List</th>\n"
       << "\t\t\t\t\t\t</tr>\n";

    std::vector<std::string> raid_event_names = util::string_split( sim -> raid_events_str, "/" );
    for ( size_t i = 0; i < raid_event_names.size(); i++ )
    {
      os << "\t\t\t\t\t\t<tr";
      if ( ( i & 1 ) )
      {
        os << " class=\"odd\"";
      }
      os << ">\n";

      os.printf(
        "\t\t\t\t\t\t\t<th class=\"right\">%d</th>\n"
        "\t\t\t\t\t\t\t<td class=\"left\">%s</td>\n"
        "\t\t\t\t\t\t</tr>\n",
        ( int )i,
        raid_event_names[ i ].c_str() );
    }
    os << "\t\t\t\t\t</table>\n";
  }
  os << "\t\t\t\t</div>\n";

  // Right side charts: hps
  os << "\t\t\t\t<div class=\"charts\">\n";

  for ( size_t i = 0; i < ri.hps_charts.size(); i++ )
  {
    os.printf(  "\t\t\t\t\t<map id='HPSMAP%d' name='HPSMAP%d'></map>\n", ( int )i, ( int )i );
    os.printf(
      "\t\t\t\t\t<img id='HPSIMG%d' src=\"%s\" alt=\"HPS Chart\" />\n",
      ( int )i, ri.hps_charts[ i ].c_str() );
  }

  // RNG chart
  if ( sim -> report_rng )
  {
    os << "\t\t\t\t\t<ul>\n";
    for ( size_t i = 0; i < sim -> players_by_name.size(); i++ )
    {
      player_t* p = sim -> players_by_name[ i ];
      double range = ( p -> collected_data.dps.percentile( 0.95 ) - p -> collected_data.dps.percentile( 0.05 ) ) / 2.0;
      os.printf(
        "\t\t\t\t\t\t<li>%s: %.1f / %.1f%%</li>\n",
        util::encode_html( p -> name() ).c_str(),
        range,
        p -> collected_data.dps.mean() ? ( range * 100 / p -> collected_data.dps.mean() ) : 0 );
    }
    os << "\t\t\t\t\t</ul>\n";
  }

  os << "\t\t\t\t</div>\n";

  // closure
  os << "\t\t\t\t<div class=\"clear\"></div>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n\n";

}

// print_html_raid_imagemaps ================================================

void print_html_raid_imagemap( report::sc_html_stream& os, sim_t* sim, size_t num, bool dps )
{
  std::vector<player_t*> player_list = ( dps ) ? sim -> players_by_dps : sim -> players_by_hps;
  size_t start = num * MAX_PLAYERS_PER_CHART;
  size_t end = start + MAX_PLAYERS_PER_CHART;

  for ( size_t i = 0; i < player_list.size(); i++ )
  {
    player_t* p = player_list[ i ];
    if ( ( dps ? p -> collected_data.dps.mean() : p -> collected_data.hps.mean() ) <= 0 )
    {
      player_list.resize( i );
      break;
    }
  }

  if ( end > player_list.size() ) end = player_list.size();

  os << "\t\t\tn = [";
  for ( int i = ( int )end - 1; i >= ( int )start; i-- )
  {
    os << "\"player" << player_list[i] -> index << "\"";
    if ( i != ( int )start ) os << ", ";
  }
  os << "];\n";

  char imgid[32];
  util::snprintf( imgid, sizeof( imgid ), "%sIMG%u", ( dps ) ? "DPS" : "HPS", as<unsigned>( num ) );
  char mapid[32];
  util::snprintf( mapid, sizeof( mapid ), "%sMAP%u", ( dps ) ? "DPS" : "HPS", as<unsigned>( num ) );

  os.printf(
    "\t\t\tu = document.getElementById('%s').src;\n"
    "\t\t\tgetMap(u, n, function(mapStr) {\n"
    "\t\t\t\tdocument.getElementById('%s').innerHTML += mapStr;\n"
    "\t\t\t\t$j('#%s').attr('usemap','#%s');\n"
    "\t\t\t\t$j('#%s area').click(function(e) {\n"
    "\t\t\t\t\tanchor = $j(this).attr('href');\n"
    "\t\t\t\t\ttarget = $j(anchor).children('h2:first');\n"
    "\t\t\t\t\topen_anchor(target);\n"
    "\t\t\t\t});\n"
    "\t\t\t});\n\n",
    imgid, mapid, imgid, mapid, mapid );
}

void print_html_raid_imagemaps( report::sc_html_stream& os, sim_t* sim, sim_t::report_information_t& ri )
{

  os << "\t\t<script type=\"text/javascript\">\n"
     << "\t\t\tvar $j = jQuery.noConflict();\n"
     << "\t\t\tfunction getMap(url, names, mapWrite) {\n"
     << "\t\t\t\t$j.getJSON(url + '&chof=json&callback=?', function(jsonObj) {\n"
     << "\t\t\t\t\tvar area = false;\n"
     << "\t\t\t\t\tvar chart = jsonObj.chartshape;\n"
     << "\t\t\t\t\tvar mapStr = '';\n"
     << "\t\t\t\t\tfor (var i = 0; i < chart.length; i++) {\n"
     << "\t\t\t\t\t\tarea = chart[i];\n"
     << "\t\t\t\t\t\tarea.coords[2] = 523;\n"
     << "\t\t\t\t\t\tmapStr += \"\\n  <area name='\" + area.name + \"' shape='\" + area.type + \"' coords='\" + area.coords.join(\",\") + \"' href='#\" + names[i] + \"'  title='\" + names[i] + \"'>\";\n"
     << "\t\t\t\t\t}\n"
     << "\t\t\t\t\tmapWrite(mapStr);\n"
     << "\t\t\t\t});\n"
     << "\t\t\t}\n\n";

  for ( size_t i = 0; i < ri.dps_charts.size(); i++ )
  {
    print_html_raid_imagemap( os, sim, i, true );
  }

  for ( size_t i = 0; i < ri.hps_charts.size(); i++ )
  {
    print_html_raid_imagemap( os, sim, i, false );
  }

  os << "\t\t</script>\n";

}

// print_html_scale_factors =================================================

void print_html_scale_factors( report::sc_html_stream& os, sim_t* sim )
{
  if ( ! sim -> scaling -> has_scale_factors() ) return;

  os << "\t\t<div id=\"raid-scale-factors\" class=\"section grouped-first\">\n\n"
     << "\t\t\t<h2 class=\"toggle\">DPS Scale Factors (dps increase per unit stat)</h2>\n"
     << "\t\t\t<div class=\"toggle-content hide\">\n";

  os << "\t\t\t\t<table class=\"sc\">\n";

  player_e prev_type = PLAYER_NONE;

  for ( size_t i = 0, players = sim -> players_by_name.size(); i < players; i++ )
  {
    player_t* p = sim -> players_by_name[ i ];

    if ( p -> type != prev_type )
    {
      prev_type = p -> type;

      os << "\t\t\t\t\t<tr>\n"
         << "\t\t\t\t\t\t<th class=\"left small\">Profile</th>\n";
      for ( stat_e j = STAT_NONE; j < STAT_MAX; j++ )
      {
        if ( sim -> scaling -> stats.get_stat( j ) != 0 )
        {
          os << "\t\t\t\t\t\t<th class=\"small\">" << util::stat_type_abbrev( j ) << "</th>\n";
        }
      }
      os << "\t\t\t\t\t\t<th class=\"small\">wowhead</th>\n"
         << "\t\t\t\t\t\t<th class=\"small\">lootrank</th>\n"
         << "\t\t\t\t\t</tr>\n";
    }

    os << "\t\t\t\t\t<tr";
    if ( ( i & 1 ) )
    {
      os << " class=\"odd\"";
    }
    os << ">\n";
    os.printf(
      "\t\t\t\t\t\t<td class=\"left small\">%s</td>\n",
      p -> name() );
    for ( stat_e j = STAT_NONE; j < STAT_MAX; j++ )
    {
      if ( sim -> scaling -> stats.get_stat( j ) != 0 )
      {
        if ( p -> scaling.get_stat( j ) == 0 )
        {
          os << "\t\t\t\t\t\t<td class=\"small\">-</td>\n";
        }
        else
        {
          os.printf(
            "\t\t\t\t\t\t<td class=\"small\">%.*f</td>\n",
            sim -> report_precision,
            p -> scaling.get_stat( j ) );
        }
      }
    }
    os.printf(
      "\t\t\t\t\t\t<td class=\"small\"><a href=\"%s\"> wowhead </a></td>\n"
      "\t\t\t\t\t\t<td class=\"small\"><a href=\"%s\"> wowhead (caps merged)</a></td>\n"
      "\t\t\t\t\t\t<td class=\"small\"><a href=\"%s\"> lootrank</a></td>\n"
      "\t\t\t\t\t</tr>\n",
      p -> report_information.gear_weights_wowhead_std_link.c_str(),
      p -> report_information.gear_weights_wowhead_alt_link.c_str(),
      p -> report_information.gear_weights_lootrank_link.c_str() );
  }
  os << "\t\t\t\t</table>\n";

  if ( sim -> iterations < 10000 )
    os << "\t\t\t\t<div class=\"alert\">\n"
       << "\t\t\t\t\t<h3>Warning</h3>\n"
       << "\t\t\t\t\t<p>Scale Factors generated using less than 10,000 iterations will vary from run to run.</p>\n"
       << "\t\t\t\t</div>\n";

  os << "\t\t\t</div>\n"
     << "\t\t</div>\n\n";
}

// print_html_help_boxes ====================================================

void print_html_help_boxes( report::sc_html_stream& os, sim_t* sim )
{
  os << "\t\t<!-- Help Boxes -->\n";

  os << "\t\t<div id=\"help-apm\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>APM</h3>\n"
     << "\t\t\t\t<p>Average number of actions executed per minute.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-constant-buffs\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>Constant Buffs</h3>\n"
     << "\t\t\t\t<p>Buffs received prior to combat and present the entire fight.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-count\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>Count</h3>\n"
     << "\t\t\t\t<p>Average number of times an action is executed per iteration.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-direct-results\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>Impacts</h3>\n"
     << "\t\t\t\t<p>Average number of impacts against a target (for attacks that hit multiple times per execute) per iteration.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-crit\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>Crit</h3>\n"
     << "\t\t\t\t<p>Average crit damage.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-crit-pct\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>Crit%</h3>\n"
     << "\t\t\t\t<p>Percentage of executes that resulted in critical strikes.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-dodge-pct\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>Dodge%</h3>\n"
     << "\t\t\t\t<p>Percentage of executes that resulted in dodges.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-dpe\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>DPE</h3>\n"
     << "\t\t\t\t<p>Average damage per execution of an individual action.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-dpet\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>DPET</h3>\n"
     << "\t\t\t\t<p>Average damage per execute time of an individual action; the amount of damage generated, divided by the time taken to execute the action, including time spent in the GCD.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-dpr\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>DPR</h3>\n"
     << "\t\t\t\t<p>Average damage per resource point spent.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-dps\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>DPS</h3>\n"
     << "\t\t\t\t<p>Average damage per active player duration.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-dpse\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>Effective DPS</h3>\n"
     << "\t\t\t\t<p>Average damage per fight duration.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-dps-pct\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>DPS%</h3>\n"
     << "\t\t\t\t<p>Percentage of total DPS contributed by a particular action.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-dtps\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>DTPS</h3>\n"
     << "\t\t\t\t<p>Average damage taken per second per active player duration.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-hps\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>HPS</h3>\n"
     << "\t\t\t\t<p>Average healing (or absorption) per active player duration.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-hps-pct\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>HPS%</h3>\n"
     << "\t\t\t\t<p>Percentage of total HPS (including absorb) contributed by a particular action.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-hpe\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>HPE</h3>\n"
     << "\t\t\t\t<p>Average healing (or absorb) per execution of an individual action.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-hpet\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>HPET</h3>\n"
     << "\t\t\t\t<p>Average healing (or absorb) per execute time of an individual action; the amount of healing generated, divided by the time taken to execute the action, including time spent in the GCD.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-hpr\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>HPR</h3>\n"
     << "\t\t\t\t<p>Average healing (or absorb) per resource point spent.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-tmi\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>Theck-Meloree Index</h3>\n"
     << "\t\t\t\t<p>Measure of damage smoothness, calculated over entire fight length.  Lower is better.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-dynamic-buffs\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>Dynamic Buffs</h3>\n"
     << "\t\t\t\t<p>Temporary buffs received during combat, perhaps multiple times.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-error\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>Error</h3>\n"
     << "\t\t\t\t<p>Estimator for the " << sim -> confidence * 100.0 << "confidence intervall.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-glance-pct\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>G%</h3>\n"
     << "\t\t\t\t<p>Percentage of executes that resulted in glancing blows.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-block-pct\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>B%</h3>\n"
     << "\t\t\t\t<p>Percentage of executes that resulted in blocking blows.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-id\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>Id</h3>\n"
     << "\t\t\t\t<p>Associated spell-id for this ability.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-ability\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>Ability</h3>\n"
     << "\t\t\t\t<p>Name of the ability</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-total\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>Total</h3>\n"
     << "\t\t\t\t<p>Total damage for this ability during the fight.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-hit\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>Hit</h3>\n"
     << "\t\t\t\t<p>Average non-crit damage.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-interval\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>Interval</h3>\n"
     << "\t\t\t\t<p>Average time between executions of a particular action.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-avg\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>Avg</h3>\n"
     << "\t\t\t\t<p>Average direct damage per execution.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-miss-pct\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>M%</h3>\n"
     << "\t\t\t\t<p>Percentage of executes that resulted in misses, dodges or parries.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-origin\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>Origin</h3>\n"
     << "\t\t\t\t<p>The player profile from which the simulation script was generated. The profile must be copied into the same directory as this HTML file in order for the link to work.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-parry-pct\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>Parry%</h3>\n"
     << "\t\t\t\t<p>Percentage of executes that resulted in parries.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-range\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>Range</h3>\n"
     << "\t\t\t\t<p>( dps.percentile( 0.95 ) - dps.percentile( 0.05 ) / 2</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-rps-in\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>RPS In</h3>\n"
     << "\t\t\t\t<p>Average primary resource points generated per second.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-rps-out\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>RPS Out</h3>\n"
     << "\t\t\t\t<p>Average primary resource points consumed per second.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-scale-factors\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>Scale Factors</h3>\n"
     << "\t\t\t\t<p>Gain per unit stat increase except for <b>Hit/Expertise</b> which represent <b>Loss</b> per unit stat <b>decrease</b>.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-ticks\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>Ticks</h3>\n"
     << "\t\t\t\t<p>Average number of periodic ticks per iteration. Spells that do not have a damage-over-time component will have zero ticks.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-ticks-crit\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>T-Crit</h3>\n"
     << "\t\t\t\t<p>Average crit tick damage.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-ticks-crit-pct\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>T-Crit%</h3>\n"
     << "\t\t\t\t<p>Percentage of ticks that resulted in critical strikes.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-ticks-hit\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>T-Hit</h3>\n"
     << "\t\t\t\t<p>Average non-crit tick damage.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-ticks-miss-pct\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>T-M%</h3>\n"
     << "\t\t\t\t<p>Percentage of ticks that resulted in misses, dodges or parries.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-ticks-uptime\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>UpTime%</h3>\n"
     << "\t\t\t\t<p>Percentage of total time that DoT is ticking on target.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-ticks-avg\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>T-Avg</h3>\n"
     << "\t\t\t\t<p>Average damage per tick.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-timeline-distribution\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>Timeline Distribution</h3>\n"
     << "\t\t\t\t<p>The simulated encounter's duration can vary based on the health of the target and variation in the raid DPS. This chart shows how often the duration of the encounter varied by how much time.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-fight-length\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>Fight Length</h3>\n"
     << "\t\t\t\t<p>Fight Length: " << sim -> max_time.total_seconds() << "<br />\n"
     << "\t\t\t\tVary Combat Length: " << sim -> vary_combat_length << "</p>\n"
     << "\t\t\t\t<p>Fight Length is the specified average fight duration. If vary_combat_length is set, the fight length will vary by +/- that portion of the value. See <a href=\"http://code.google.com/p/simulationcraft/wiki/Options#Combat_Length\" class=\"ext\">Combat Length</a> in the wiki for further details.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-waiting\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>Waiting</h3>\n"
     << "\t\t\t\t<p>This is the percentage of time in which no action can be taken other than autoattacks. This can be caused by resource starvation, lockouts, and timers.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<div id=\"help-sf-ranking\">\n"
     << "\t\t\t<div class=\"help-box\">\n"
     << "\t\t\t\t<h3>Scale Factor Ranking</h3>\n"
     << "\t\t\t\t<p>This row ranks the scale factors from highest to lowest, checking whether one scale factor is higher/lower than another with statistical significance.</p>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n";

  os << "\t\t<!-- End Help Boxes -->\n";
}

// print_html_styles ========================================================

void print_html_styles( report::sc_html_stream& os, sim_t* sim )
{
  // Styles
  // If file is being hosted on simulationcraft.org, link to the local
  // stylesheet; otherwise, embed the styles.
  if ( sim -> hosted_html )
  {
    os << "\t\t<style type=\"text/css\" media=\"screen\">\n"
       << "\t\t\t@import url('http://www.simulationcraft.org/css/styles.css');\n"
       << "\t\t</style>\n"
       << "\t\t<style type=\"text/css\" media=\"print\">\n"
       << "\t\t	@import url('http://www.simulationcraft.org/css/styles-print.css');\n"
       << "\t\t</style>\n";
  }
  else if ( sim -> print_styles == 1 )
  {
    os << "\t\t<style type=\"text/css\" media=\"all\">\n"
       << "\t\t\t* {border: none;margin: 0;padding: 0; }\n"
       << "\t\t\tbody {padding: 5px 25px 25px 25px;font-family: \"Lucida Grande\", Arial, sans-serif;font-size: 14px;background-color: #f9f9f9;color: #333;text-align: center; }\n"
       << "\t\t\tp {margin: 1em 0 1em 0; }\n"
       << "\t\t\th1, h2, h3, h4, h5, h6 {width: auto;color: #777;margin-top: 1em;margin-bottom: 0.5em; }\n"
       << "\t\t\th1, h2 {margin: 0;padding: 2px 2px 0 2px; }\n"
       << "\t\t\th1 {font-size: 24px; }\n"
       << "\t\t\th2 {font-size: 18px; }\n"
       << "\t\t\th3 {margin: 0 0 4px 0;font-size: 16px; }\n"
       << "\t\t\th4 {font-size: 12px; }\n"
       << "\t\t\th5 {font-size: 10px; }\n"
       << "\t\t\ta {color: #666688;text-decoration: none; }\n"
       << "\t\t\ta:hover, a:active {color: #333; }\n"
       << "\t\t\tul, ol {padding-left: 20px; }\n"
       << "\t\t\tul.float, ol.float {padding: 0;margin: 0; }\n"
       << "\t\t\tul.float li, ol.float li {display: inline;float: left;padding-right: 6px;margin-right: 6px;list-style-type: none;border-right: 2px solid #eee; }\n"
       << "\t\t\t.clear {clear: both; }\n"
       << "\t\t\t.hide, .charts span {display: none; }\n"
       << "\t\t\t.center {text-align: center; }\n"
       << "\t\t\t.float {float: left; }\n"
       << "\t\t\t.mt {margin-top: 20px; }\n"
       << "\t\t\t.mb {margin-bottom: 20px; }\n"
       << "\t\t\t.force-wrap {word-wrap: break-word; }\n"
       << "\t\t\t.mono {font-family: \"Lucida Console\", Monaco, monospace;font-size: 12px; }\n"
       << "\t\t\t.toggle {cursor: pointer; }\n"
       << "\t\t\th2.toggle {padding-left: 16px;background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA4AAAAeCAIAAACT/LgdAAAABGdBTUEAANbY1E9YMgAAABl0RVh0U29mdHdhcmUAQWRvYmUgSW1hZ2VSZWFkeXHJZTwAAAD1SURBVHja7JQ9CoQwFIT9LURQG3vBwyh4XsUjWFtb2IqNCmIhkp1dd9dsfIkeYKdKHl+G5CUTvaqqrutM09Tk2rYtiiIrjuOmaeZ5VqBBEADVGWPTNJVlOQwDyYVhmKap4zgGJp7nJUmCpQoOY2Mv+b6PkkDz3IGevQUOeu6VdxrHsSgK27azLOM5AoVwPqCu6wp1ApXJ0G7rjx5oXdd4YrfQtm3xFJdluUYRBFypghb32ve9jCaOJaPpDpC0tFmg8zzn46nq6/rSd2opAo38IHMXrmeOdgWHACKVFx3Y/c7cjys+JkSP9HuLfYR/Dg1icj0EGACcXZ/44V8+SgAAAABJRU5ErkJggg==) 0 -10px no-repeat; }\n"
       << "\t\t\th2.open {margin-bottom: 10px;background-position: 0 9px; }\n"
       << "\t\t\th3.toggle {padding-left: 16px;background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAwAAAAaCAIAAAAMmCo2AAAABGdBTUEAANbY1E9YMgAAABl0RVh0U29mdHdhcmUAQWRvYmUgSW1hZ2VSZWFkeXHJZTwAAAEfSURBVHjazJPLjkRAGIXbdSM8ACISvWeDNRYeGuteuL2EdMSGWLrOmdExaCO9nLOq+vPV+S9VRTwej6IoGIYhCOK21zzPfd/f73da07TiRxRFbTkQ4zjKsqyqKoFN27ZhGD6fT5ZlV2IYBkVRXNflOI5ESBAEz/NEUYT5lnAcBwQi307L6aZpoiiqqgprSZJwbCF2EFTXdRAENE37vr8SR2jhAPE8vw0eoVORtw/0j6Fpmi7afEFlWeZ5jhu9grqui+M4SZIrCO8Eg86y7JT7LXx5TODSNL3qDhw6eOeOIyBJEuUj6ZY7mRNmAUvQa4Q+EEiHJizLMgzj3AkeMLBte0vsoCULPHRd//NaUK9pmu/EywDCv0M7+CTzmb4EGADS4Lwj+N6gZgAAAABJRU5ErkJggg==) 0 -11px no-repeat; }\n"
       << "\t\t\th3.open {background-position: 0 7px; }\n"
       << "\t\t\th4.toggle {margin: 0 0 8px 0;padding-left: 12px;background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAoAAAAVCAIAAADw0OikAAAABGdBTUEAANbY1E9YMgAAABl0RVh0U29mdHdhcmUAQWRvYmUgSW1hZ2VSZWFkeXHJZTwAAAD8SURBVHjavJHLjkRAGIUbRaxd3oAQ8QouifDSFmysPICNIBZ2EhuJuM6ZMdFR3T3LOYtKqk79/3/qKybLsrZteZ5/3DXPs67rxLbtvu+bprluHMexrqumaZZlMdhM05SmaVVVhBBst20zDMN1XRR822erJEnKsmQYxjRNz/M4jsM5ORsKguD7/r7vqHAc5/Sg3+orDsuyGHGd3OxXsY8/9R92XdfjOH60i6IAODzsvQ0sgApw1I0nAZACVGAAPlEU6WigDaLoEcfxleNN8mEY8Id0c2hZFlmWgyDASlefXhiGqqrS0eApihJFkSRJt0nHj/I877rueNGXAAMAKcaTc/aCM/4AAAAASUVORK5CYII=) 0 -8px no-repeat; }\n"
       << "\t\t\th4.open {background-position: 0 6px; }\n"
       << "\t\t\ta.toggle-details {margin: 0 0 8px 0;padding-left: 12px;background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAkAAAAXCAYAAADZTWX7AAAABGdBTUEAANbY1E9YMgAAABl0RVh0U29mdHdhcmUAQWRvYmUgSW1hZ2VSZWFkeXHJZTwAAADiSURBVHjaYvz//z/DrFmzGBkYGLqBeG5aWtp1BjTACFIEAkCFZ4AUNxC7ARU+RlbEhMT+BMQaQLwOqEESlyIYMIEqlMenCAQsgLiakKILQNwF47AgSfyH0leA2B/o+EfYTOID4gdA7IusAK4IGk7ngNgPqOABut3I1uUDFfzA5kB4YOIDTAxEgOGtiAUY2vlA2hCIf2KRZwXie6AQPwzEFUAsgUURSGMQEzAqQHFmB8R30BS8BWJXoPw2sJuAjNug2Afi+1AFH4A4DCh+GMXhQIEboHQExKeAOAbI3weTAwgwAIZTQ9CyDvuYAAAAAElFTkSuQmCC) 0 4px no-repeat; }\n"
       << "\t\t\ta.open {background-position: 0 -11px; }\n"
       << "\t\t\ttd.small a.toggle-details {background-position: 0 2px; }\n"
       << "\t\t\ttd.small a.open {background-position: 0 -13px; }\n"
       << "\t\t\t#active-help, .help-box {display: none; }\n"
       << "\t\t\t#active-help {position: absolute;width: 350px;padding: 3px;background: #fff;z-index: 10; }\n"
       << "\t\t\t#active-help-dynamic {padding: 6px 6px 18px 6px;background: #eeeef5;outline: 1px solid #ddd;font-size: 13px; }\n"
       << "\t\t\t#active-help .close {position: absolute;right: 10px;bottom: 4px; }\n"
       << "\t\t\t.help-box h3 {margin: 0 0 5px 0;font-size: 16px; }\n"
       << "\t\t\t.help-box {border: 1px solid #ccc;background-color: #fff;padding: 10px; }\n"
       << "\t\t\ta.help {cursor: help; }\n"
       << "\t\t\t.section {position: relative;width: 1200px;padding: 8px;margin-left: auto;margin-right: auto;margin-bottom: -1px;border: 1px solid #ccc;background-color: #fff;-moz-box-shadow: 4px 4px 4px #bbb;-webkit-box-shadow: 4px 4px 4px #bbb;box-shadow: 4px 4px 4px #bbb;text-align: left; }\n"
       << "\t\t\t.section-open {margin-top: 25px;margin-bottom: 35px;-moz-border-radius: 15px;-khtml-border-radius: 15px;-webkit-border-radius: 15px;border-radius: 15px; }\n"
       << "\t\t\t.grouped-first {-moz-border-radius-topright: 15px;-moz-border-radius-topleft: 15px;-khtml-border-top-right-radius: 15px;-khtml-border-top-left-radius: 15px;-webkit-border-top-right-radius: 15px;-webkit-border-top-left-radius: 15px;border-top-right-radius: 15px;border-top-left-radius: 15px; }\n"
       << "\t\t\t.grouped-last {-moz-border-radius-bottomright: 15px;-moz-border-radius-bottomleft: 15px;-khtml-border-bottom-right-radius: 15px;-khtml-border-bottom-left-radius: 15px;-webkit-border-bottom-right-radius: 15px;-webkit-border-bottom-left-radius: 15px;border-bottom-right-radius: 15px;border-bottom-left-radius: 15px; }\n"
       << "\t\t\t.section .toggle-content {padding: 0; }\n"
       << "\t\t\t.player-section .toggle-content {padding-left: 16px;margin-bottom: 20px; }\n"
       << "\t\t\t.subsection {background-color: #ccc;width: 1000px;padding: 8px;margin-bottom: 20px;-moz-border-radius: 6px;-khtml-border-radius: 6px;-webkit-border-radius: 6px;border-radius: 6px;font-size: 12px; }\n"
       << "\t\t\t.subsection-small {width: 500px; }\n"
       << "\t\t\t.subsection h4 {margin: 0 0 10px 0; }\n"
       << "\t\t\t.profile .subsection p {margin: 0; }\n"
       << "\t\t\t#raid-summary .toggle-content {padding-bottom: 0px; }\n"
       << "\t\t\tul.params {padding: 0;margin: 4px 0 0 6px; }\n"
       << "\t\t\tul.params li {float: left;padding: 2px 10px 2px 10px;margin-left: 10px;list-style-type: none;background: #eeeef5;font-family: \"Lucida Grande\", Arial, sans-serif;font-size: 11px; }\n"
       << "\t\t\t#masthead ul.params {margin-left: 4px; }\n"
       << "\t\t\t#masthead ul.params li {margin-left: 0px;margin-right: 10px; }\n"
       << "\t\t\t.player h2 {margin: 0; }\n"
       << "\t\t\t.player ul.params {position: relative;top: 2px; }\n"
       << "\t\t\t#masthead h2 {margin: 10px 0 5px 0; }\n"
       << "\t\t\t#notice {border: 1px solid #ddbbbb;background: #ffdddd;font-size: 12px; }\n"
       << "\t\t\t#notice h2 {margin-bottom: 10px; }\n"
       << "\t\t\t.alert {width: 800px;padding: 10px;margin: 10px 0 10px 0;background-color: #ddd;-moz-border-radius: 6px;-khtml-border-radius: 6px;-webkit-border-radius: 6px;border-radius: 6px; }\n"
       << "\t\t\t.alert p {margin-bottom: 0px; }\n"
       << "\t\t\t.section .toggle-content {padding-left: 18px; }\n"
       << "\t\t\t.player > .toggle-content {padding-left: 0; }\n"
       << "\t\t\t.toc {float: left;padding: 0; }\n"
       << "\t\t\t.toc-wide {width: 560px; }\n"
       << "\t\t\t.toc-narrow {width: 375px; }\n"
       << "\t\t\t.toc li {margin-bottom: 10px;list-style-type: none; }\n"
       << "\t\t\t.toc li ul {padding-left: 10px; }\n"
       << "\t\t\t.toc li ul li {margin: 0;list-style-type: none;font-size: 13px; }\n"
       << "\t\t\t.charts {float: left;width: 541px;margin-top: 10px; }\n"
       << "\t\t\t.charts-left {margin-right: 40px; }\n"
       << "\t\t\t.charts img {padding: 8px;margin: 0 auto;margin-bottom: 20px;border: 1px solid #ccc;-moz-border-radius: 6px;-khtml-border-radius: 6px;-webkit-border-radius: 6px;border-radius: 6px;-moz-box-shadow: inset 1px 1px 4px #ccc;-webkit-box-shadow: inset 1px 1px 4px #ccc;box-shadow: inset 1px 1px 4px #ccc; }\n"
       << "\t\t\t.talents div.float {width: auto;margin-right: 50px; }\n"
       << "\t\t\ttable.sc {border: 0;background-color: #eee; }\n"
       << "\t\t\ttable.sc tr {background-color: #fff; }\n"
       << "\t\t\ttable.sc tr.head {background-color: #aaa;color: #fff; }\n"
       << "\t\t\ttable.sc tr.odd {background-color: #f3f3f3; }\n"
       << "\t\t\ttable.sc th {padding: 2px 4px 4px 4px;text-align: center;background-color: #aaa;color: #fcfcfc; }\n"
       << "\t\t\ttable.sc th.small {padding: 2px 2px;font-size: 12px; }\n"
       << "\t\t\ttable.sc th a {color: #fff;text-decoration: underline; }\n"
       << "\t\t\ttable.sc th a:hover, table.sc th a:active {color: #f1f1ff; }\n"
       << "\t\t\ttable.sc td {padding: 2px;text-align: center;font-size: 13px; }\n"
       << "\t\t\ttable.sc th.left, table.sc td.left, table.sc tr.left th, table.sc tr.left td {text-align: left; }\n"
       << "\t\t\ttable.sc th.right, table.sc td.right, table.sc tr.right th, table.sc tr.right td {text-align: right;padding-right: 4px; }\n"
       << "\t\t\ttable.sc th.small {padding: 2px 2px 3px 2px;font-size: 11px; }\n"
       << "\t\t\ttable.sc td.small {padding: 2px 2px 3px 2px;font-size: 11px; }\n"
       << "\t\t\ttable.sc tr.details td {padding: 0 0 15px 15px; 0px;margin: 5px 0 10px 0;text-align: left;background-color: #eee;font-size: 11px; }\n"
       << "\t\t\ttable.sc tr.details td ul {padding: 0;margin: 4px 0 8px 0; }\n"
       << "\t\t\ttable.sc tr.details td ul li {clear: both;padding: 2px;list-style-type: none; }\n"
       << "\t\t\ttable.sc tr.details td ul li span.label {display: block;padding: 2px;float: left;width: 145px;margin-right: 4px;background: #f3f3f3; }\n"
       << "\t\t\ttable.sc tr.details td ul li span.tooltip {display: block;float: left;width: 190px; }\n"
       << "\t\t\ttable.sc tr.details td ul li span.tooltip-wider {display: block;float: left;width: 350px; }\n"
       << "\t\t\ttable.sc tr.details td div.float {width: 350px; }\n"
       << "\t\t\ttable.sc tr.details td div.float h5 {margin-top: 4px; }\n"
       << "\t\t\ttable.sc tr.details td div.float ul {margin: 0 0 12px 0; }\n"
       << "\t\t\ttable.sc td.filler {background-color: #ccc; }\n"
       << "\t\t\ttable.sc .dynamic-buffs tr.details td ul li span.label {width: 120px; }\n"
       << "\t\t\ttr.details td table.details {padding: 0px;margin: 5px 0 10px 0; background-color: #eee;}\n"
       << "\t\t\ttr.details td table.details tr.odd td {background-color: #f3f3f3; }\n"
       << "\t\t\ttr.details td table.details tr td {padding: 0 0 15px 15px; }\n"
       << "\t\t\ttr.details td table.details tr td.right {text-align: right; }\n"
       << "\t\t\t.player-thumbnail {float: right;margin: 8px;border-radius: 12px;-moz-border-radius: 12px;-webkit-border-radius: 12px;-khtml-border-radius: 12px; }\n"
       << "\t\t</style>\n";
  }
  else if ( sim -> print_styles == 2 )
  {
    os << "\t\t<style type=\"text/css\" media=\"all\">\n"
       << "\t\t\t* {border: none;margin: 0;padding: 0; }\n"
       << "\t\t\tbody {padding: 5px 25px 25px 25px;font-family: \"Lucida Grande\", Arial, sans-serif;font-size: 14px;background: #261307;color: #FFF;text-align: center; }\n"
       << "\t\t\tp {margin: 1em 0 1em 0; }\n"
       << "\t\t\th1, h2, h3, h4, h5, h6 {width: auto;color: #FDD017;margin-top: 1em;margin-bottom: 0.5em; }\n"
       << "\t\t\th1, h2 {margin: 0;padding: 2px 2px 0 2px; }\n"
       << "\t\t\th1 {font-size: 28px;text-shadow: 0 0 3px #FDD017; }\n"
       << "\t\t\th2 {font-size: 18px; }\n"
       << "\t\t\th3 {margin: 0 0 4px 0;font-size: 16px; }\n"
       << "\t\t\th4 {font-size: 12px; }\n"
       << "\t\t\th5 {font-size: 10px; }\n"
       << "\t\t\ta {color: #FDD017;text-decoration: none; }\n"
       << "\t\t\ta:hover, a:active {text-shadow: 0 0 1px #FDD017; }\n"
       << "\t\t\tul, ol {padding-left: 20px; }\n"
       << "\t\t\tul.float, ol.float {padding: 0;margin: 0; }\n"
       << "\t\t\tul.float li, ol.float li {display: inline;float: left;padding-right: 6px;margin-right: 6px;list-style-type: none;border-right: 2px solid #333; }\n"
       << "\t\t\t.clear {clear: both; }\n"
       << "\t\t\t.hide, .charts span {display: none; }\n"
       << "\t\t\t.center {text-align: center; }\n"
       << "\t\t\t.float {float: left; }\n"
       << "\t\t\t.mt {margin-top: 20px; }\n"
       << "\t\t\t.mb {margin-bottom: 20px; }\n"
       << "\t\t\t.force-wrap {word-wrap: break-word; }\n"
       << "\t\t\t.mono {font-family: \"Lucida Console\", Monaco, monospace;font-size: 12px; }\n"
       << "\t\t\t.toggle {cursor: pointer; }\n"
       << "\t\t\th2.toggle {padding-left: 18px;background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA4AAAAeCAIAAACT/LgdAAAABGdBTUEAANbY1E9YMgAAABl0RVh0U29mdHdhcmUAQWRvYmUgSW1hZ2VSZWFkeXHJZTwAAAFaSURBVHjaYoz24a9N51aVZ2PADT5//VPS+5WRk51RVZ55STu/tjILVnV//jLEVn1cv/cHMzsb45OX/+48/muizSoiyISm7vvP/yn1n1bs+AE0kYGbkxEiaqDOcn+HyN8L4nD09aRYhCcHRBakDK4UCKwNWM+sEIao+34aoQ6LUiCwMWR9sEMETR12pUBgqs0a5MKOJohdKVYAVMbEQDQYVUq6UhlxZmACIBwNQNJCj/XVQVFjLVbCsfXrN4MwP9O6fn4jTVai3Ap0xtp+fhMcZqN7S06CeU0fPzBxERUCshLM6ycKmOmwEhVYkiJMa/oE0HyJM1zffvj38u0/wkq3H/kZU/nxycu/yIJY8v65678LOj8DszsBt+4+/iuo8COmOnSlh87+Ku///PjFXwIRe2qZkKggE56IZebnZfn56x8nO9P5m/+u3vkNLHBYWdARExMjNxczQIABACK8cxwggQ+oAAAAAElFTkSuQmCC) 0 -10px no-repeat; }\n"
       << "\t\t\th2.toggle:hover {text-shadow: 0 0 2px #FDD017; }\n"
       << "\t\t\th2.open {margin-bottom: 10px;background-position: 0 9px; }\n"
       << "\t\t\t#home-toc h2.open {margin-top: 20px; }\n"
       << "\t\t\th3.toggle {padding-left: 16px;background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAwAAAAaCAYAAACD+r1hAAAABGdBTUEAANbY1E9YMgAAABl0RVh0U29mdHdhcmUAQWRvYmUgSW1hZ2VSZWFkeXHJZTwAAAD/SURBVHjaYvx7QdyTgYGhE4iVgfg3A3bACsRvgDic8f///wz/Lkq4ADkrgVgIh4bvIMVM+i82M4F4QMYeIBUAxE+wKP4IxCEgxWC1MFGgwGEglQnEj5EUfwbiaKDcNpgA2EnIAOg8VyC1Cog5gDgMZjJODVBNID9xABVvQZdjweHJO9CQwQBYbcAHmBhIBMNBAwta+MtgSx7A+MBpgw6pTloKxBGkaOAB4vlAHEyshu/QRLcQlyZ0DYxQmhuIFwNxICnBygnEy4DYg5R4AOW2D8RqACXxMCA+QYyG20CcAcSHCGUgTmhxEgPEp4gJpetQZ5wiNh7KgXg/vlAACDAAkUxCv8kbXs4AAAAASUVORK5CYII=) 0 -11px no-repeat; }\n"
       << "\t\t\th3.toggle:hover {text-shadow: 0 0 2px #CDB007; }\n"
       << "\t\t\th3.open {background-position: 0 7px; }\n"
       << "\t\t\th4.toggle {margin: 0 0 8px 0;padding-left: 12px;background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAoAAAAVCAIAAADw0OikAAAABGdBTUEAANbY1E9YMgAAABl0RVh0U29mdHdhcmUAQWRvYmUgSW1hZ2VSZWFkeXHJZTwAAAD8SURBVHjavJHLjkRAGIUbRaxd3oAQ8QouifDSFmysPICNIBZ2EhuJuM6ZMdFR3T3LOYtKqk79/3/qKybLsrZteZ5/3DXPs67rxLbtvu+bprluHMexrqumaZZlMdhM05SmaVVVhBBst20zDMN1XRR822erJEnKsmQYxjRNz/M4jsM5ORsKguD7/r7vqHAc5/Sg3+orDsuyGHGd3OxXsY8/9R92XdfjOH60i6IAODzsvQ0sgApw1I0nAZACVGAAPlEU6WigDaLoEcfxleNN8mEY8Id0c2hZFlmWgyDASlefXhiGqqrS0eApihJFkSRJt0nHj/I877rueNGXAAMAKcaTc/aCM/4AAAAASUVORK5CYII=) 0 -8px no-repeat; }\n"
       << "\t\t\th4.open {background-position: 0 6px; }\n"
       << "\t\t\ta.toggle-details {margin: 0 0 8px 0;padding-left: 12px;background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAoAAAAWCAYAAAD5Jg1dAAAABGdBTUEAANbY1E9YMgAAABl0RVh0U29mdHdhcmUAQWRvYmUgSW1hZ2VSZWFkeXHJZTwAAADpSURBVHjaYvx7QdyLgYGhH4ilgfgPAypgAuIvQBzD+P//f4Z/FyXCgJzZQMyHpvAvEMcx6b9YBlYIAkDFAUBqKRBzQRX9AuJEkCIwD6QQhoHOCADiX0D8F4hjkeXgJsIA0OQYIMUGNGkesjgLAyY4AsTM6IIYJuICTAxEggFUyIIULIpA6jkQ/0AxSf8FhoneQKxJjNVxQLwFiGUJKfwOxFJAvBmakgh6Rh+INwCxBDG+NoEq1iEmeK4A8Rt8iQIEpgJxPjThYpjIhKSoFFkRukJQQK8D4gpoCDDgSo+Tgfg0NDNhAIAAAwD5YVPrQE/ZlwAAAABJRU5ErkJggg==) 0 -9px no-repeat; }\n"
       << "\t\t\ta.open {background-position: 0 6px; }\n"
       << "\t\t\ttd.small a.toggle-details {background-position: 0 -10px; }\n"
       << "\t\t\ttd.small a.open {background-position: 0 5px; }\n"
       << "\t\t\t#active-help, .help-box {display: none;-moz-border-radius: 6px;-khtml-border-radius: 6px;-webkit-border-radius: 6px; }\n"
       << "\t\t\t#active-help {position: absolute;width: auto;padding: 3px;background: transparent;z-index: 10; }\n"
       << "\t\t\t#active-help-dynamic {max-width: 400px;padding: 8px 8px 20px 8px;background: #333;font-size: 13px;text-align: left;border: 1px solid #222;-moz-border-radius: 6px;-khtml-border-radius: 6px;-webkit-border-radius: 6px;border-radius: 6px;-moz-box-shadow: 4px 4px 10px #000;-webkit-box-shadow: 4px 4px 10px #000;box-shadow: 4px 4px 10px #000; }\n"
       << "\t\t\t#active-help .close {display: block;height: 14px;width: 14px;position: absolute;right: 12px;bottom: 7px;background: #000 url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA4AAAAOCAYAAAAfSC3RAAAABGdBTUEAANbY1E9YMgAAABl0RVh0U29mdHdhcmUAQWRvYmUgSW1hZ2VSZWFkeXHJZTwAAAE8SURBVHjafNI/KEVhGMfxc4/j33BZjK4MbkmxnEFiQFcZlMEgZTAZDbIYLEaRUMpCuaU7yCCrJINsJFkUNolSBnKJ71O/V69zb576LOe8v/M+73ueVBzH38HfesQ5bhGiFR2o9xdFidAm1nCFop7VoAvTGHILQy9kCw+0W9F7/o4jHPs7uOAyZrCL0aC05rCgd/uu1Rus4g6VKKAa2wrNKziCPTyhx4InClkt4RNbardFoWG3E3WKCwteJ9pawSt28IEcDr33b7gPy9ysVRZf2rWpzPso0j/yax2T6EazzlynTgL9z2ykBe24xAYm0I8zqdJF2cUtog9tFsxgFs8YR68uwFVeLec1DDYEaXe+MZ1pIBFyZe3WarJKRq5CV59Wiy9IoQGDmPpvVq3/Tg34gz5mR2nUUPzWjwADAFypQitBus+8AAAAAElFTkSuQmCC) no-repeat; }\n"
       << "\t\t\t#active-help .close:hover {background-color: #1d1d1d; }\n"
       << "\t\t\t.help-box h3 {margin: 0 0 12px 0;font-size: 14px;color: #C68E17; }\n"
       << "\t\t\t.help-box p {margin: 0 0 10px 0; }\n"
       << "\t\t\t.help-box {background-color: #000;padding: 10px; }\n"
       << "\t\t\ta.help {color: #C68E17;cursor: help; }\n"
       << "\t\t\ta.help:hover {text-shadow: 0 0 1px #C68E17; }\n"
       << "\t\t\t.section {position: relative;width: 1200px;padding: 8px;margin-left: auto;margin-right: auto;margin-bottom: -1px;border: 0;-moz-box-shadow: 0px 0px 8px #FDD017;-webkit-box-shadow: 0px 0px 8px #FDD017;box-shadow: 0px 0px 8px #FDD017;color: #fff;background-color: #000;text-align: left; }\n"
       << "\t\t\t.section-open {margin-top: 25px;margin-bottom: 35px;-moz-border-radius: 15px;-khtml-border-radius: 15px;-webkit-border-radius: 15px;border-radius: 15px; }\n"
       << "\t\t\t.grouped-first {-moz-border-radius-topright: 15px;-moz-border-radius-topleft: 15px;-khtml-border-top-right-radius: 15px;-khtml-border-top-left-radius: 15px;-webkit-border-top-right-radius: 15px;-webkit-border-top-left-radius: 15px;border-top-right-radius: 15px;border-top-left-radius: 15px; }\n"
       << "\t\t\t.grouped-last {-moz-border-radius-bottomright: 15px;-moz-border-radius-bottomleft: 15px;-khtml-border-bottom-right-radius: 15px;-khtml-border-bottom-left-radius: 15px;-webkit-border-bottom-right-radius: 15px;-webkit-border-bottom-left-radius: 15px;border-bottom-right-radius: 15px;border-bottom-left-radius: 15px; }\n"
       << "\t\t\t.section .toggle-content {padding: 0; }\n"
       << "\t\t\t.player-section .toggle-content {padding-left: 16px; }\n"
       << "\t\t\t#home-toc .toggle-content {margin-bottom: 20px; }\n"
       << "\t\t\t.subsection {background-color: #333;width: 1000px;padding: 8px;margin-bottom: 20px;-moz-border-radius: 6px;-khtml-border-radius: 6px;-webkit-border-radius: 6px;border-radius: 6px;font-size: 12px; }\n"
       << "\t\t\t.subsection-small {width: 500px; }\n"
       << "\t\t\t.subsection h4 {margin: 0 0 10px 0;color: #fff; }\n"
       << "\t\t\t.profile .subsection p {margin: 0; }\n"
       << "\t\t\t#raid-summary .toggle-content {padding-bottom: 0px; }\n"
       << "\t\t\tul.params {padding: 0;margin: 4px 0 0 6px; }\n"
       << "\t\t\tul.params li {float: left;padding: 2px 10px 2px 10px;margin-left: 10px;list-style-type: none;background: #2f2f2f;color: #ddd;font-family: \"Lucida Grande\", Arial, sans-serif;font-size: 11px;-moz-border-radius: 8px;-khtml-border-radius: 8px;-webkit-border-radius: 8px;border-radius: 8px; }\n"
       << "\t\t\tul.params li.linked:hover {background: #393939; }\n"
       << "\t\t\tul.params li a {color: #ddd; }\n"
       << "\t\t\tul.params li a:hover {text-shadow: none; }\n"
       << "\t\t\t.player h2 {margin: 0; }\n"
       << "\t\t\t.player ul.params {position: relative;top: 2px; }\n"
       << "\t\t\t#masthead {height: auto;padding-bottom: 30px;border: 0;-moz-border-radius: 15px;-khtml-border-radius: 15px;-webkit-border-radius: 15px;border-radius: 15px;-moz-box-shadow: 0px 0px 8px #FDD017;-webkit-box-shadow: 0px 0px 8px #FDD017;box-shadow: 0px 0px 8px #FDD017;text-align: left;color: #FDD017;background: #000 url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAUUAAACXCAYAAACC9UnNAAAABmJLR0QA/wD/AP+gvaeTAAAACXBIWXMAAAsTAAALEwEAmpwYAAAAB3RJTUUH3gMHEhQHkf+gfgAAIABJREFUeNrsvXe4XWW17/+ZdfW6e2/pvZIEAgkJodfQBUSkKKJiAUUQD2ChKVVRRECKgJBQpYWQEAgJ6T17p+ze11579TbXbL8/8Pjz3HvOuadcfTz32Z9/VpnrmWs+Y47xneMd453vFBhjjDH+j5R7nLEz50wMOS0Hp1x5Dv5QJW17d+LWcsQTRV7etJOBkSjdwyOCPWau/9EIYyYYY4x/P0LOnT/N/vZ5i4jbOayCRsrOM33cZMTBCG90pJgUsjnn6u/gdJfx6eqXWP7tuwTN+rcDThDAtmFMPMdEcYwx/qEoUaR74ha3WKb5b/7m2duvsWeEi7y1t5ft7SMohSSBgEp7xKS2KkhIdrCvy2L5zBCnnXUBM2bPoHtogDNOuZDDmin8r8r39AWn2b9c8yE7UsWx2BsTxTH+lvjdDkIO1Q75Pay8ZAWDR/vp6u7nwMEOenL62Hn+V/jV1y+3+1Jp7n72jf/NPrIk8eSNZ9rF0S4e+jSCmRIIeQDJRTJXpKLUTTyZopi3GRmNcPK5lxBySYz2t/LA3T/hzRee54ePvbBjxLDm/fM+Vwbc9s1fvZzvPPY0m9Nj52RMFMf4m6CIAhdM9dh/ane+0uAsXqgb0J1KEawIccn5Z/Glrx5P+9pd7GntxxVqJNbXyVvb9tHZ1StouvGX/YiC8BeHkCURRAEQELCxESgW9f/2cE/4N97bf/5s/9X3Nn/74eVJ48rsJx74Kc3nfEWw7X/5b7/+8nH2gdZdvNNXgs8wyFoatqDSl8rS4HWAZiI5VArFIpMm1rFpzxEUwaSkpIJJoSKvPvcsC8+4kp2Dw4Jp2YjAqm9cb+cHe7hs1dtjcfcPjDxmgv+5VzNJFJnZNN7+rGOYBlfhwqxuIzmdTPeWICsuHnvsWWoUnVPOnIGoabiJ4b/gas7LpHGGq+yP1nxEx44uqlrctFQEMFEJeoOMrzQRPEHwVCNaBplkgV2/f45vv/juirhtr/3vHLf9169/VkIBsP6X7X+1+W/GJ0dHhIBdsBvLS+zO4ehfhOrCGVW2oqf5416FMl+OtGlRkFycYxv87MeT+N4Tg6xuj1HtcTCkqXgPdvDF00/gjbWfkU4k+DiS5Ylnn+GNR++k4aKvYVomZR6nfe7NZ3Djosv//6LiGGOZ4hj/PuJ/MkOaVCLbqmcCipUink4i2hZFyyTs89E/kuT7P/s65UIG0yhQHMlQroLk96KX1lBeUU7V1AVIsoyRLRDv6wM9g1Ixi44DnXS1d1A0UtQ3BDln5WWYUoHO9W9y/3fv4A/dqf8n/MYPPHnxArv61AtYfNXNf+kaP3D2RPvhdZ04HE4GDZELKspwKDF++fZzJPJuQvJR1qxv5b6bnuW+WxYwpSHCCy9FeaXPw3BXL1HLwue02Pj8o1z7g4d5c/tu4dUHb7ZLd37Gt97cxrZkYSzuxkRxjP8IkiBg2fZ/WBRFQWScr8z2BGXy6RyWYOKUHaQMWLhwAopXoDxUgpYyqS21qVOdxC0LyRWkpkzFW1lDsHkmIa+HhOni3guuoK2nA0GDMp9E3bhqPjs8yOypk7nzyefxK3l0ReLo63/k6z9/lrbhqFA0rf+x9nYpAtcFJfuhD19m0lnf5FB3nyAKcFxdmT2Q0iiYNidVu2h2e/jR0ytJWSsR5CJ2sgfETvz2CF2tfZgjCYJWklvfzfKFqT6uWd3BcCzJD687jRNnLebFNW/z4+tP5f6vPcLqWJb9sdxY3P2DJydj/INg/icEEcDnEDEcEj6HgiQLCLpIwtDwOWzskEImGWX/gTZCngzOcBW7RlIUJJO6SSUMqE1IoTqKmsW+rR+Tz0V5aOP7vNk9wAUP3ou89Hg+HSoy75IrqJg2k1vOXsiLqx9nNJlm4vITWf/pn/jgpXvsG8dV2f83ajANHpf94pfPsz3i388lVQs0ySKx4VVWP/ZzAgE/351fYScQkSWBaEHnugVlHNsiEusvoNhbkLPv45xUQfL1Fziy5zCZPYdwqBkG0hruHCRHs9Q6RdweDy9/sBtMgS9ccREb/7iJ9VmTgOIYc/SxTHGMvxVBt6O5oX5ce3/7Abw+P7ZgI5sSDpdMRVUFghNcgoWv1Icv5CUkl6GLRSrLa3H5XBSGRihtrqJ5xgqGhw8zqakJ1eVGysQomTgdwcihFQrYskQxZzPQtp3RyD5KaqdRGarFpyokBJH41m08+vjveW5r23/Kn05oabJPOmYmp19yFrMXr2DVH16gd+N6bnr5/b+bX36l0Wu392Z4/JffJqJ5WPPe07yws0gmn0USJF46vp5MeZ76iY14Z4ewXu8kbo+j9NIw0sBhCn0aBz/pxulW+WhfhpggcTSl0ZYskjM03nvqFzz78NOM13r4p3adKU6Zj4diY3E3Jopj/C0ol4mteumXod8+8w4fffwRDlXGthRk06CsthqjkCWnF/A5FCrqyqmbMA5Ts4hHIyioKKEAy4+rpP/ICCWNszjY0UZzy0RqAxLOTJHqky7AqaXJZDXyWpyAMwCmRTo9hNshEHJKoMtkcEIuyfBwlCcffIp32o7uGMlk5v1rx1zjdtpzZ07ln264hjlnLSMnymzf8Bm//uFduL1ethzu4EAk/nfzy2/UuexB0c0JTWVMqHbzSm+OdW0jRNNZrp/iY1xRZs7F1SiygPZRBO+ZpxCqzlGMd5Pvtogl87TtHqErkcZyhtjYnyZnFonrNkPJPD/5yhfw6Tk+WP02WwUnc4NeXj7aNxZ3/8CMdZ//BzO5yh+KxxI0B20Ohr0kUlkEw6YgwHBvP16Hiq3lSXsDaH0xEl2bSOYL2JKAr6wKdzrPL3cf4bhTJlNdUk/ZYDfLz72Ud+++CQSFLUc6cDaU01g9joCnHEuPo5smXqefQnaUztEkPqeCahu43eVUNtbxsz8+zt3x+NxPn/yjvWrHVnozJl63m1IFvnXlhQQnNOOaPp+BTzfx0+/cxWdvvYFHVakorcQbdtAd+/s2cdYnipzot3h0a46HTqplULMR9SJep5OasIN4XCPvqiG9tZPq7x1P9r2dxCIqppAn3WWiBGSsvMYxc+pI9GXZlVSQEjoZUcAhKXy041Nuvfgyfp57A8VKMGPuAl4+2jfmvGOiOMbfpAZpSQwlVfYNx/AGSskU8ti2iFMS0RBIaHncqgOHpZPpTpBURZxeB/3RLHa8m8qAi2B5mI/e201Xe5awV2Xvjl0svfpmRre8QH++yIG+OG27X2HylFlMaxmHz+0jlyliajqpgoVuZXAKEpLfiWkb6PFR3A4Xp91+I8tzGkVLRCokKSCRl2x62hO8dNKZJJMR/BisqAtSr4h8ZmiUeFSyhvl3tWGp00HU6yaUt9iS0zk6YmHZAg7ZIpt1cOkPT2fw8FFM3WTwF5tw11cguCWsdAlaaQbbtpErfAz0RTF1maDD5thFPiIjAj/fEaU9AUooyISKEIpZIN63C0GUsC1zzIH/QRlrtPxrNQUBptfKttch0FQm23XlDntynd+u9Ct2qVd6ubLEZXucMookoCjSX1UiBED6K7NK/63KhigKCP9OhSOez5DLpliycBlu2UK1JYqmiS4YlMkK1aqCz9Y5PJrg97cu5LITm+lN5LntouP4zTUL0UfzRAYi5BIZ2vfuYziZ491X3+DjTa9jNMzGU+qloTaMZbvo7e0jlc2RBwQrQ7aYp2CKyC4ZzRQYHOlmaDRCLB1jNBWls2uAXD7PaCJJb6pAf6LA+rdbuefii8jmEtS7HDQGPfhlGMlnmTWuiY93HsHpUHEqMh6HggTIooBLVXDIMooookgSsiQiSxJOh4rboeKQFRyqivLnCeghn/skVZJwqZ/vSxEFRFFGFkVkUf4XFi1aJiFd5uSJfrIZmdFYHEEEo2AxebyCWOpB0N24ZlVTd8Vi1IklFGwfBamArBXJ9KUJZXI0jK9mwlQ3AZ+X4ypUJjsNlraE2dfWQTLWy+xjT6TWbZEoSlwypeVvNklRACQBAi4Zr1OirlS1ywNqrNwvxabWOewyn7J9XK3XDnjG8qGxTPE/gW0LnDxJ5tIvOu0PDqqkMyJOBHylNpls4cLSUj8Bn213dSepKavCcsGmnV3E0jJLF01g4YxJbNqznWNmzqPgsbBtHWM4gVjuxVk7Hj02SqG/B8e06SS2bsNTG0TvSCI3VeNyedBTCUrcNit/+Bmd3Qnhy2eMt6/64gRMxUN+fx+KrpLRi+iGwhOfbsWXHaK3uw9BUHAoCtUuDw5b5+PhBE9+cwUHjwyxe9cAy2dUcOcbBWr9Sbx183j8mkHOeGKASneS9gGD4ZEc9U1BsvkZGEKOkKRQUioye0ozG7Ye4EDHEerTMaqbJqHIFlrvfuKhBQTUUbwKoJh0D4yghCdS5c/SPpxBkl1UV5fz1Mv72PLrOwnVV2FqRUYQQRQpkxVyuky2JMzr228h2m3bhpxDcjUwsu9jJEMjOH02xVgbeiSB5FXpXbcVy+WgobEW99SJGDkX2f5enCEn9z7wDqdfeDozJ5SDR8V0VqANHWZg6wHKJ0/EyObYt3knmWyeVKaAN21yzfQgk37fzuK6MiRTQpAhEA7xi49S/Kx6F54SB8r4Orre20u2QqQai0jcxqm66I4McPatC9jwi+1UnzGLrxb28vymAh22gGpJ2MCRSIap42t59l2d8gYvV86p4+qfnG/LwRDFoZ3YOQOGe8ChIDgElJZZ6FYSvbWTdCSK0ypQyNmU1pRjTZiOVUhiFyrIp7roOdCN7HQT9LtonjOL8pBM5+YNzJlVxh1PdvGNs4PIoje0Y98RKmobmTgxPFczYNL5r/G3nx7/PzQpGjPBv20aSYSTJqv2dy+eilRWwd2/3YO/OEJX3KJ31GRps0C8CAG/g2UzvdRUKew+lOEPG/MoqkwyrdNcFSBTNIkls5iCREu1l2SygKqodCcMQrJGEZGJVR4SOQtJVrBMgYBXpG2gQOdAXDhmYtAuGiamoCJaOtmCTSZbJOxUcZc14zRTxEdGKeomgiAj2Abzy1WuX1zLT7aN8tC1Uzjmlo10/nEJ4bPXMH/GeB48JkGxfib3P3OQde3DPHLjPN7fGOXTfR0sPfsUmiaV43F7aK4Okk/FKVhetu4/yjEzJlBfV4NZtMgkR1C9DmTRRZnPhZkZwV9aQjwt09YawVnRzI4dnex/61nkwigESvCTo6CB2+2i1Mxz2kkn4Dl2Kjfe8gv6Illqq7yMJjSmj6/FRMU0NDK5FEOxHAGPC9nSSOQl/B6IpzRiWQ2HZDKxtJybLjmL067+KiNDGWTZxD1hEm/dfQdi2INk2Gxas55Xd+/DVxJALxrIbhcr/SKXnxDipKeGOXdeCa/uiBByiKhOFUuWKOTyZLNFblxeydfPq2XrtgLFTIJPPopz5e0r2PPTlzjmrqV0rRnG44yw4tkCpR4Dt6JgGAbRnMYV55/C0umNdG3vZtv2LeTrQkTy4FZMtGKecDjAYN8AhiUyEsthINMQdlEACsU80bhGZciBppn4Q0GGRzPMmljC1EqVs89diDmaRqoK86tHPuDE2QHe2tjOhweLXHtaJWv3GiwZL3H2ydNwW1nO+/GOHSOp4ryxu2rGRPG/ZBj7z+ap8dj2I18fz6G4lyc+6KWzM8qCySXU1pez+v1WZEAFckCJBLVlEPJKzK8X2Rpxk0pbTCgrYuoWAxkvffEsomXiVS0iGZHrrlqMX1UR3V62bD7McPcwhq0QKRbJpJI88OOL2bG1nbKyAL1DGezRGN2pPEPdg0xvCfFJu021W6BzNMXkhjL+6aQKetIp7vsoQyIySqyg4JN1QrWlNAYcbNvez60XNXL+jFoO63kuv38nJSEvj3x3AmfctIVxDTXMOn4KjS2NOFSZ+io3+UyeVMGJKMqUBtz09XRREoC6qlqiWYGwC1wuH9GRDB0Hh5hy3ExW/XoVe/Zux4NNUZLwiuB3OinaBkVTY3rdZK656xJuvPj7zLt0BSuWzOTBh17hyitO4Ee3P8eX57hZc9SiUDQ47ZSZlDXVkI/mCIwr5/3VW5jaoNA+qrN8ylKmTJrKrsN7aGttpWgVUVw+uvb2smzJZHr39+B0OjnhmhuY3VzPw089zJbtW1CKWapzBj9b7GXKKxlOm1nB+3sGkWWRiV4FTbDpzebxSwpHExkWBwOcPtFLIRqjQ/Zz0+kNXPPITu75+lzuf30bb3eqlHqhxe8mkUvjkGVGshrHLlzA9CYfX73qu3zvyi+QDkiUKnn2D1qc9YVTSMdTyKqTPYc7cPQfpD1tk87JpHIWp88pob4xzKe7+tjepXPdcTI7e0wUdHZ26hQNyGoQVCCmg+RzccmJjXy0qYsj0Txv37ccr2pyqCPJdY/sEkQErL/cYD4mimOi+F9AkaBCxl5yTBnHNWuU1Uzkpoe2kdfh2stmsuIYP7u6bH7zu42UugUsVaHEJbO3M091ic2CRomOIZvFEyTebJOIxXX6MxYByUYXBTTLRhAVdMvG4xJRnTKFeB6vQ6A3DQ4FnA4FqWiiYxPyQHvUpMYlMq/SIqaBt3EGw0c6iBkC2743hVs/TNLWHuPoYIoSp0TOtBBQcKgCclFHEmBXSueZK0qZOPEEYoPdnP6rbZw4oYLDQylEQ+OML19MIT2ErDiYOW8urvwgo5qLRCLLpMmTyBQ0PO4K6sM2Tn8pg/17sYsxAuWz2fjRdj783Ut4fQ4GEhmckkipS8ZEJew2meZV2HYgyu3PfJ9f/uQ37B4cZnf086Un/A5IaxCWBEzRpjEImiHQmrARRBGnaVGUZVQMplWGefiOm/nlqlcx4wM0TJlGOFiBx+8gGYmiZAs4QiUItoYa9HHw488Y6Yzy622HeeZbK/nNn9bhcirMLKliZV2I+3pHGBjOMZQ3uXt5LZOqUnz7tSL1PpsHrlvBgy98wotdUUKeMBfUBejJFcDjYPWOHqr9DuZWuPnatDBfW9vBtFI3jR6BuGbQoVYxs6WCM+eOR8v5efbZ+zmcg96cRF3YhSDLRJN5Sn0SWi6H6hZJRC0kRaI3bXH7RU30do3wcVuOSVUeDKVAz6iT8bUqaiGB1y3x+t4irz3+Fdq2bOR7z3fhL2RweEWumQ1RXebBDUVBFsH4e6y0MSaK/+83XQKK0Jwo2h0/uLDaPrRrlNZMkVhG4psrffzk5RRXzBMp8VioqsTbm3VaakXitp8frND4zQaNM2Yq3P2WRk1FkDmVRZprLNp7VYLuIlVlKh0DAg3VXgbwI0f7CZUpdGct7n0xwffPcFETLkPHgeQO01jhoqGlkqORAu6KME0lGZ598SgfftLBSNFi3QOnc9Zta9GTOrqWx+9USeY0/E6FRodMn1ZksGBiyCrTVZM588u55cIWXt0sc90T71Mf9CNJMqUON8dfuZiR7lG8bpi/eA497UNEEymOW7yc4aEBGhsm4vWFkMUiIZeMbugMDibp7TrC+mde50j7IZw+P16nC0kQGecVafHZ9CVVTvvhd2n0ZdEcezB1GZevDAMvzWVVfLBuLbMmwtDgKEe6HMxvziHWjiMak6mrC9KbiFGyvotHD6t09bUyc+ZUfB4vqtOJw6OjFZ20rtnJjFMWUEhlSCaS1JSo5HICckHnwMbPuPhHT5Luf5zta/cTaYsg1dWxuS9JMqVjiTJPXdLMtFuu5MA1v+WjtkOsKqoYgxlmz6njqbsX0REV8SGz66FVvJNWCHpdbBzO0xyQuWBRHWY6TySS5pnDKdKmxOTJkwi7VP7pRz/g2Xt/RiyscUylxoh/CnY6QU1piOhADw2+JG1yM5XWKMlhnUUnT2PJRc+yZtWX+eb3PuYPL17Gtlc2kJa9bHxjNdUNYYKqyO5UOflkhGhK5fZr5nPdD97gl3ecgd+lc8kdGykhz86oPRbvY6L4f0cVBRuW1Nl2pnwcX1lu0X9omPvfzrF8Ery53wZEzp0lcP6xVQgeP6veOsh3vjaJr/20g4BHZHqzzdY9GmesqGDL7hjdQwa1JSrINqmUTcgvoiouvF6ToWgRTbeZ2ezm+c80vrzEwWBKIZ0oki4YJDQRW1DRdAufV6YgOZjgD1AXELigJsVDn4lMDZu83BYnJNqYiFS7RFKmjduyaS9AXhRYFFQYLghsH4gxeXotN10/n0ishgfv/T2KCpLi5axvnE9/6yFKQ1V4ywKUBVSGR7OossCM8RMxRBFB9BIKl+KVdIKBIJKg09pv8c7zb0IuiWt4N7plEDBNVEmnxO3nxJ/9lmmhBGdfcSO9RYPDHQM0VIawBRvdtKjyO7G9PoqJDL5wmPaj/Xi9AkXj8263L6fR0lhPIR+nqaGOysYqvA4Xlp1CliU2bezinC9eRHxoiPRIJ0O9EcY3TmDPQITmeDdKOEz0QBdzv/VT3vvp7fQW4oTLQmzuTGOaOg6Hk58c66MpYKJ+8URy2w4R+WiE2dfMw1VM8uFLMRSnztRjy+n6sIeHDyQIZBNceeUSXL2dhH1FNnUUGI0X2ZctciijY1gB4oUirz38QwxRZv0zd7EpblFZW45sFSiYEpJRpHdIp6ZSIpsHQXQSCAWYN8nDe6/t5Lk1DzJz2g289IcbkUb72bKzlYnjJhKaVcfsRfdT6bbwhELMqXPg8Pn50TeO57PDOV54chVtoxZHR0zBGssS/49IYyb4j1w1BBpLHHf0DWV5a1OC8+Y6GYlpDOteLlvo52uXzuDRN/tIaV5WLm+hvaeHH/w6Qm/W5NqVNTy6ehR30IutlOEwc7TUOpjSXElJqR9L13G7XBx72hlkEnkEt5sVJ51AITiO2qDMsXMmIctB1HAFzePrqa2uw+HzUlkWwFdSis/vZYaoM+gVOTbg442Ek1ZXA45ILzNcIjFJYroDCqqDoaKIgsB4t8ywpTBSLDL9uLlE2nt587X9bN2+A6/ThY2FqoBtKVhOJ1axgKgGaRo/nUR/G2Yxiy0JuIPlpJJDKMg4XA6wJNxuP+ObSpm39Dg+eWsV3nApqupA1RLIqsKSqy6jsSbA1268hZjTTUNViFOPbaSuthktl6CluYWZ48uYPb0WyVbw+FS+9YUTaD3Uz6wmF7PGtaBIFnaxSKCkDJdLQrAlgl4bTbXYtzXCghULGentQvDIRPtSOBQRd3UZxUwOT2MzluhAqKhnx6oXmX7779j/yu+QAk56RosIiIiCSUemSKgoktl5EDtrY+kaoWiOu1fHuOrh0/HGeig54RyG9m9kTyzPz69o4pn1XXzhgin86oU2evIC64Z06qd7ae/KksgZzBhXxe69HZxy2inoPWmSZh5HuJqaihpypkpDfQmxWIqFs6oJBUOYuTRbD0XpH0qx43CEo609JPIadz+6huhAP/NnTaB+SoD5J/+GG740nZuvWkzbkMX7e4a4ZaWXkZjI+be8wfKJHjqHCkQL3DkW0WOi+N9GlQVObFHt5UvKeHdzDMOyqKsKMGXRRB44T+K2Pw7ywpoh2h6fwhPrNTqPtlLu97G3p0CVW0CxTWZXe/nqxeOYUGpyyeUNPPCbLt7dG+OeW5YwvbGEBbPmMHU+9G3ZSFNTCI8Tbn/wPR64bRmaXqRESrNgiodbfraWi4/1Ui4VmFUjsWKeh6ef30JXTiNsGFx0YSN/3FbAr6dJJJI4nE7cssKerMVw3kAyTQKqjSAJOASQG+uZ1nuUnGXThMGy6RMoE3VURWJAExH1Iu6qKmK9EUoqyrEEk7wuEu3voKLEg6gl8XgDdIwMoOd16usbGI0lGBoephgbYcnpZ5AXbVz5PA2nncOXbr+NyppqPv3lLdQ2yyyf7uGuRzdiqH7Cisahja3c/K0TaXAbHF/voLHBxd33vo+zxs14KUXr4X5aasvp7ElQ1VBJwCdjWhIOj8Ww5mVkwMmChZXkUjqFbIaBgTgCRQzB5si+I9iZIpmjHXhqaoiMjtBy3GKGNjyLcOo3ObDubTRBxRZMHLLFaM7iYBYCsoyzfYQJx43jD+/1c9mpTVxw6Uv09KQ48OSbfCh50eIGv900jFE0iR4a5oRKN69qNZSWZulKWgz3Whh2ERmZnuggib7D1B1zDEp0hKM711FeZvOVm0/FERnhrIvm8c17PqZ72yEWLKzhzuumoVbMZV5Vmhc2dNPiTuKTBD44WuShn51DwwmPseHdh9i1uZOv3/8u9325klR/L8su/yLnX/8HqrwiZ15wApI3gIp5x0A0PyaMY8Pn/xqiIIAgcO0yn727XeWWs21uXZ0maJosOy7AL99JM2N2Bdcva6bCPsJPX4qwd1RmNF7g+qUK7lIfb29Ks/rblby5O8fLnyQYFxbJajJfWBbgSERlzUENURAoD4f5bHs746odDMY1RMlidnMFPYksYZeDWDrPaCxLOOAkZwi4BRtVkUhoNvPmNKAN9GO7y/jpl2ay7IZ3yasihm3gUVzo+RxTqoKUdQ4w7eK5vLupF3cgQCKZJdEdo9xdxOvwcNk1C2ly27RtGeD1XR10ix4Er5eKSZNx+30UMnGWn342+eheYof2UdsyDrffg9NXh9cXoKAVyWYzTGmZjogAkk1+uA8NBbQ4DU1TGNfUSHz/ep55eTXvfLyTjC5x3RdP5NVV6/AHg4xEE+zszHLCDD/OUDU+2yBUGiYz2kfHcI7xbi/701mmTplAwOtGkhRsBCwsLFNE9bqRHArpkQJy2AVFyBcyiKaBM29ioeEQAVlF8KiYog/bthlfPZP7X34OYXQEw9DwelzUu2xaMxaniTmuvOMHtG5dwznfWIkZnsbA66/zu7tX017UKKlWUQyVeWUWdbXlPLVrCHd/hB+uuZtjFvyA0lAZpqAjWCKJfI6zFk7hUFc/13zpJCh48RTi3PHcahYe00LBcCKlM7gCKt/60mz2HOlj97ajtKadLD2mmdPnNLDrcA/bDmUIOUvYeGAnwwcHCUwZx0++OIe5py3jW7e/z5PPv8bBD29i2ck/pyb8YwZzAAAgAElEQVQEV123lKaaen73/Ae09aRo7c8K/7ym+hhjmeJ/+FJh27Biqtc+NOpl+1tf4OzvbmDx+CCax8eufXG6kxbv3tbEx5vauOG3I3TEwSfAK7dW8PrmJDNrJI4MiHzSDrs6NZbMbWbzviiTx3u555URuiJZsCCbKyKZOtOqdXwem6YKkQm1TqyCzvwZFXT3pJnUEqS8xIVTkQjLEqZpIkkiQdmitSNJMOAGl4fjGgNsHHSgJtJcUKnSmbOYtewk2toP8+PvXEZzAxxXGmDRrArmHNPMT26bz8y6SRgOmVzfCE1uicfWHCLqcCEIKnnLwtR0YpFBhKRGyYQWor196IUkqgyWKSNKGi7ZScEQMGw3mqETcCq4HQ403SbRuZtQzQTqa2vQc1nUxrm4WvfRkRyi2ify/vZemitcFHWTpqZKjp0YIGW4GO4fQnbJjHT3kTZ06n0iEcMi7C3D7VLR0wVsUUC3LEQgpxkU8kWG2g6RMW30dJbR9v1Ee0YwdIucbWG5XGTzApZkkctqaNkkfsnHhrd+w0lnXMxH6zbgcLgocRlMLXMyq0wlPZDgwh9fxuCvnsMZdGKPbMIz92SWXjeNxpFe5IiTEjcsO62BTdtHaCpz0Z8RufTy6ez8ZA+ZAmAJKKoT0bawZRmfz4ua1xiMduJ0hrnhomWs2dZGe1+U0UyORLrAo89uZV+/xpSKCqoUESuX5VsPrKU7WmR+g4vZF36Vo7s/4YZvf4my/nVc9sAehvfu4/tXL6ZrME6kJ8a21j5a997JovOf5ooVNZTNXsBMc4gRU75jYDQ3ljGOieJ/prci8MFvTrA3bkvw4R++xCln/oKMJtPYGOaNT/ood9osrbN47O0Yrb15StwCH9zXzO83JDly1ODUs46lNFDCth39FHSbrR1Zth2Mks5Db5eGbdmkchajySKxtE5PNEvPoEl7v0FXr8H+jgI7+zX27I2SiObZfzTBvq40scEs3SM5SOuMxD8v5GcyGlm1jHPmhsmWFMn6F+GodrJ5VzuEA/h7d3PZlBZmN9hYogcKQ7yyNsqegx18877duDsPc8rdt7Pt2Rd4rdskqlukshoOp0quaKH6A4TLK4kNjzBh7hxUO4EllyCLJpbDSWxwBNntBdPAzCWIxhJYgomVL+BWLGxspEKE8uYZhFwqnds/ZeGNd9PVk6atL0c6nqOtJ0P3QJbWw1F2H4kz2JsgGy9S7E+xN1qkf1ijTnGC24nsClDdUocNuLxe3H4/R/f0c+x5i9nywXau+sXjNNQGOPb8i5lz0pksOvM8euPDJPr7iCVSSG43LpcLQZawZBlFTOGrnIBbhoKjlHQyhkuWmFLpojMjML9WZsbSBWzYcJCqhjSOvEBi7zqMvh7ECpl5SxWmLhnPhrd6cDl1dEklr7lYfHIt73/Qii9dQBclCoCiOjjaE2HBpDr2D8SZM7WZjqNHiR/YyVeu/BqbtrTRcbSfntEcgiGQimfZcWiI99tG+GT/CDPDAidP8ZE2XbiCOidMr+byax/nvGuvIW9ZVCfaWJfzEzmwg+ZgDCNXQBEUxjtE7n5+C88/diVP7Svj+kvK+GzbwB3JdPHOsWxxTBT/Q/zpkYX212//lN89/WXef3YVj3yY5OZLjmd36xEumy/i8jnY0WlQFrDw1rfwzWUyF9/Xy9UnlyGaeX7/py5Wb+olmTVJJnTqJBEZG48goAgQlkRkQaBCAqcIfkFAlWzqFQG/JCAJMEsFE5Fax+d3UpcrAi5ZRBYAEZx/frCUIgk4MJk2uYwdHWUsPH4But+Ld98HBKMZFNXH9x64Frn7MI8/u4tvr++nwVFk8rITefTa8fx+Swc3futSXnnrIypcHnZGskyocqA6nWSMz/ef0TTcThnB4cZd2sxI935UbykBpwOHMwCWiS278Ho8xHs6cRsmhmBTSCfRESiraiYf7aJ60kz8DpU9H7xPLJahQhFQJaiQJVyqQFAWcYsCflnEJ4kgi/hFEZ8AJdUeHC4fottDNBrDtCCXTHPiyusZiW1m2tKvcfF1l5Eb6qK6rpnE0DDZdJa+I9uZNH4CZRNaUCQBUYTeI0PIioXPX86oBe6An8iGd5h19Xf54NU/4nYplMgmHQmdcxbVUtrk5tCmQ/QdHkXO5RFxEzs4gJTW6e/UGDiSJ2XmKSRMotEkcmkZkyaWsG57G0IGcrZMQrNorCwlmsoiywqCaFKuyriCJfR0DzPSvpNLV55GfjSJHBvFQsAlCBgW+EWBgCTQnxF4pzXJnoNDZPYd5O2PO3n+11/kgQdeYv7kuUy8YCW/uOtpls+oZMf+GK/87tvc9IsPuOGLx7L2gMb9D7/O3ZdWcv9LMe788kQ2HozekUjk7hTGqmhjovjv8cxPTrIffno3jz50G9vWvMfdzxzmG5fPZe++fYTCXm64ZDr3vdzD9AqT9w4L3HbReG56+ghnzPDw8idxDvQZ+ICwLVIuQkD8XOREQSAo2oiigEP83NHdCFQr4ASCokjOtNFt8Irgkj5/NIFmglMUcYsg2QJORUSxbIoW+ESBvGnjK2oUamYya/58BhIREof3cu7559CyZAaz5o/jrm/+gkxCYp8gcevMFjb0pbnrtmm8s2ofQn8ni69Yye7166ixCgzmFKa0+ElbRaJpC1EEQ3BSUltCVVUVWUFENkCwcpj5NOVVJaQ1ASOXIK8VcCpeUvkCPrcHEZNQ0I8sCJSW1VAc7iI0YS6OQ5vZvbcdVRJxKCImNnbRQtNtbNMmGArhkwVKfUFM22JKyEXnSBxfwE/WBgSZqtpGvE0NhBwZjj/zespCBkbeoG8kjmVYJNJZUvFeJFEmU9BJJzN4/CG8pX4EocjAYATDzOAWLRKWk6xbJlTM05c0UPIZZMEAy2DOhBrCFT6yRyMUFp7GaGwUJZcHlxtD1xE9Ipt2x/D29HDST27n5796m5u+fSJDu9poT47SMyxRMEzSRZjSWE5fJIEgQFnYTyqfp64qgFd2kY7EybUf5vTTV7D7cA/RVBZBsHEKAi4BFGwkIPzni+G+lEmDO8dzf9zBuSunUHSodG1cw4UXnUv/4W1kLBPdX8mv7v0RSy68jccevI7P1u+jdSDGa09cxclfW8OaNx9j3fodd0Si0bGh9F8xtiDEX/H6fcfY9z2/i1sfeIj9z9/D9144Su+q06m9+F2uX1rOnbetZMr5T1FqGuyNSjgkmz0jAgsnl/PqlmFmuAQqnCDakNJsCraNaAu4HaCKkNRAtkCUwCPZHNVt0gY4BYGwbGPYAqUOyBs2eQN0wCNBzrRxija2CCIWYRdogoBp24QRiaUsSj0qWcmmv3eATCRPgytLOtHPey8dZM75Z/Lqa2tZNK6RI5E+br1xGRuf30Z5mZd3hQoS8QwrFjTy9cd2sLQqyNp9Q0yq9mAaAoYgIhTzKKJKPJai0N1NsLwWXTcZtQTMvjz1Xo2MqdDZ0UedV8UyDTqTcSr8fgrxYVpmLaJQzFFZWkrhyA5Ouu5GPth8lKNd3RQsi3KXi7rF8/jSzd+ntrKcsubxSPk8ttePrGsIipNDbTt5+Ps3s2XPTm6/75ts3trHZdfcQNjt4HDXPnqPaBS1NJZuMZJPkUimkC2ZRDqLWYxBPoXgL2dqyyxk1YcvFOLD9R9TV1tFuSNC0eFkV8dBTj31VN5/4VfkLCeCJNMZ15hetBhO5Fmw8nw237eFza0xnAWdWsFHOqBQmYLD3iYa2toYkb2EOg7w3KYj4PcTyWg4HSKKJHCgK0LI7yarFbEtiaHYKLlUCZKikIvneXE4y/Yjz/Gday7nwSdfpDOexfXnBZcMW8S2QccGW6BJtOkZgrgksundXQwFGjl1chV3/vw5akrdfGOZh4cefY1JYY2u9Xex7PIHuOHC6RwdElhx1u3sXvsoTcdch5EZEtwOhZymjwnAWPf5fzfFkuMa7ORQmrWPLGPmFW/z+PVlfPXJKD88w8OUOZP49R8OkuuPEq72Ei/AjBqLj/tFDh3McV6TD6+dB10nmRUwDRunG4qAmRdIWgpBl/n5PDhZwO3zkM6aeEQTj9silwXTAo9LQLMdpM0CFHLkZQ+Kr5RyWcMWDXJZDQQDSzexBDAsCy1p0HDpl4jlC1x70XLue24dxvMvsuSUKZjOIFfdez09777Ehp3DaKabSfUmA30RrObl3HvnEzz5u6spDCX52e83MaHS4LNuEYcokdY0vD4PpqiimhpNxy0i1deLr6yGyoogDp8bd6yH/kgSl5SlrGoc+1sPsmDGTLREnKYqN1NOuBCnU8SVG6Bi+vGY+Tymv5H2NQ/Rt72f2vOv4sjmtXy45m0iso/evm6q65pIplI4fEGs+CANTZM5/5JLOf3M8yikEmTyQyiKk6N9gwwm8ghGgVwqysGDBwmWlJFMxHBbOYbiozgkmenTl9PX3wnFOKIrSFVVNYd2rUNRwmzYvB2PI08xl8ZU/Yz3BHlv826mlzrQzCKNFeVcPc3DH9viDPcVufqLC3E1TOSNF9Yx3pHlcCSND+gYLXB8U4AmX5D3RlL0GnG6Imkm+oJs6MuAIVLQLUrCXkYTWRqaqnFToNzjZlJLNf1tnRwpyFixCFVGnnNPXcqv/vAOpiyimzZFG/ImmALIApi2gGWBJdoULJjWbDMwCo/8eglLL9nAoS3f4ntfeZpdQxrr/vRjDr+3hhff+YQJC86lr6+N0xaGuOKObTTWVtDRMyRks7mxbvTY8Plf4vG4uHRZ6R3nL6ph+/4ovT099GfLOHWqg1Mvv4I3Pu3jmdWtXHXrqezcNUBKg3japrcnS0gDp17AKFroRdB0qKqCom6DLZI2LWTTIhBUyRd1EjmwDINCNo+maWi6jmnoFPI6lqZTLOaxCwZOVUATDQ4dSZCJZtCzOWSriJ41UCwbWzcRDAi7bLTKeloWnkiZEcEXSuC95Fq2rH4br1ukpkVmNJGnLOAhoxdwhcpZu36AiVMngdGOQ3FTWedn984BEjrUey0GEjq2KKIXssiSglMRcTgVFKeDXDxK5bgpWIUcfSkROd5PJFHElEUmTlvEYH8fsxYuYtaZl1Ne6aHEJ+KfvARbcmP7/UhCDFf9Itbu3MfTzz7Bpwc7EWpacLr91NTWomJSNX0uflknVDsB0yOy/rVVvPTck6TTeRYtOY21O3eSyuQQ8zFy2RQd+/cQ7e8nOthPaVkYzRBZftzp7Nq/m/6eTk4/9TTeevMNOvfspP/AftwN4xAEg4qGerZ/up3yEidWYCJ6ogdQyWQi/GjeeHYNZDic0pnidnD1kjpeencfzz3zLtsGo3xwZJiD0TT7RrN0aBbru+K80jFMndukRnBRqogcHEhhGwYZS6ZgGgS9HjLZHG6Xgm4KFPI56ktKceZSxLs7aQy5aGwIMDA0wuLJ4+geHaZg2FgInwerAKYlYFs2DhkMQMSmfVSkJCzw5ps9vPXkhZyy8il+98pNfPjSZh547B1u/dl1WAmNNe+vZe6scXz5ro/4cNU15Cw/Xzh3xh1/+mDv2BB6TBT/OT/8/GnL7z14or12W4aLF/q57P6dnLBsGTWBIhedeQz7dx7k3t98zJSWEJOmevhkR5qrTq7myT/1kykIzHQLVPkEfF4b2xKwANESKFqgSRKKZZMyFNyKQV6zCaoWpm4R9ggYoo2Z/3xOpIWALQpYgoDbI5DMQroo4HEI5IoCDR6wdBFLgIIhgAUuVSCbscnXteCfOA5poJfF86bxuz9twZlPYmEzvqjR7fdDNs7hPQW6jwzQOpJBsQdpqm/m0529lAsW+4YSjCRMRgoWomUiWSaipDKaTlDVWIstqZiGiOVQsLJJZEHB1C10h5Oy+ha8psbMJSuYsnApy0+ch6jYGEUdy12BKkEsOYpWyLF76zBXnr2UwUyKgNtFKOwn5Pfglkxy/b2MJCLE9h0iFRnEWV9KSFdwVwbx+Lx0HNzP7Td9l+9++/vs2rSBvJ6lde8h+jsP0TB5GhOapjLc10s6FSM6MspwZz+iYLF61R8oczuYuuh4TIdFZXk5/pJKHr/jHgLlZaR0k5m+CF1plYC3hP1dnZRi8ZV5Zcx2mGQzeR5Zu5/enEZ3USQo2ZSUhikL+FF9Thy2BaKNIkkMZU12xNMcSZiYlkHSBMuSEAQbVZbRtAIupxetUCSXz7NgTjOjQ0nqhBTJaBKrkCRYDflCDq2QRxRAKVp4FBHLBEmwMRGQbVBlMA0BvwIHkjCx3GbVR/3cfveVXH7ZEzzx21vYvPkTVr+8iZ///EZee2UduepJHN11lKvOnkpNXTlDnZ1sPRD5tFAodoxJ4tjwGYCzT2iwy10CN11zMp9sP8S1920k99lVLD3vKT5+84dMO/0+RLvID1cG+MoLGisnKLy1N4lhCJSKNscFBBySgNMDomFjFkFWwLIEDGyc0ueLkOnC5+LmlG2KhkjBNLEtgaBXJJo0EUQBv8tGy0LBFggFBbLYdEZsgghkDfCrAhUOG9n1+STuogWiaeM9cxkzT7mQD595iYcfuYonf34PzuO+weq776HB72LpolqG+yO8saOPuScs4bIT59G6cTtVIQflXg97WvsJ63lkh0BHX4InD3ejCU5Mh4qtm1TW1eEtq2Vo9CiyFKSuuhpbt9G1LFPnz6KmPMDysy5GynXgRCCiSfiEPILDjyLASDLNhP+PvfeMt6ss137/z6izz7n6yirJykrvlUCAUALSFEQF6SJWQFQsW9yWbbBiV0RsgCBIVZAgvSUESO8JyUpZWb3O3kd93g+T993nnI9nv+/RvfeZX8an8fuN+ZvjueZ9X9d1X/fMufzsrmd587HvEWvvxvcc2ufM4cDWrRzdd4Sc7aKqKprmIxQDRULVdcDzmTenmyWLFmHFAxSGUxzauJFvPPgsG+/8LGd/8qsMjR6FvMdgfy8XXPkhHrvrl2QLBcLBCF3LliD0AEtWn8bmp+5nxQXXsvulZ7nnNw9y4aXnsu3NzUyZ2U1DKEtAAaHN4MCeXRRdnybp4QYU8iWBokrC0uDLV0/lqcpcwtUMlUKJyaMj5KtF1i5IsGlPkmSpjKYqqEJQcWRtEAAdy3GJRkNUyhYNdTFcVVDIFjl5ZgvTmhuRvQMkxwdobxekU5Joi47reThVn7GSYDKjEFF9chYoClRsga5JdCEoeBJfEYzaku5OQTxWh5pIcKznOLdd1s51dw8jj3yXky/7Fded08zWXDsb17/FwMZvcPM9u/n8pz7E3HmXi5rXwf//QfG/82falIT86HlTSOWi/OjLi7jsc0+zYEYDR4dS2LbGdz69lNNvfIHvXdfIw5tt3jcf/rChRLngIyTUCVgah3oTpC9wPEnFB13UMhajUXCcWmST6wtsx0dTBIYp8ADDkZjBWjyWdCW6CooGrhRY0scWgsmCICYlMR1cBEKV4ELKFSgutJg+7Z+8iUjbFMaTWRa57zB7xSzWPzPM2tMb+cbtT3Pe6jmc9YH3U3lrO2PHB9h6qIdBTUc4LuMVm4CiUDUCeOUSQU1lEkEgHEfBp1y1mTJzFtFgjInhE0QSTXS015Gu6nzg8gtobqon1tCCUhkiagTIORoJUxA2FDTdrO2zDtbx9ENPs/6xewkG49RNbWaif4D9B07guj6aoaFpSm1Nq9BRZG3iwnN9PASu7yFUWHHyScxsjzAxWWasp5fvPPQ4v//eV1i06mTUcoZXXtlI31iKW77yVUzTgUCEd/btQDouZbtKx7RFbH32WUIBm9nLl9Mxdzkndmzhib+9TEdLgNXdYY5nYqTGsqQmRtAMlZawwbQIbBlzuGB5jIvOrOf2JwTxsIL0BdmhIUY9h6+tDHB4wmP/uMpQLo/jSsq+j4KK7UlQVcK6iuvXrFXNzQ2MTibB97n+glWUx3JUjuzBkQ5oAiPgE9QErg5OCSwE4ylBruTjCfA8gSUkDaYgbUkcBGVfMuDA4mbJslMXIHTJBefO47bb/kooEePB332SVef/lJcf/AzrH3uSmcuX8t4Lz2bV5d/n4vedzF2/ffG/PSYo/7xoXbMeCKHQEOCONcs7/4+wwLddOp0/vTzCrVcE+OTXnueKs5v52RM9vLA1yVc+uowNW4ZwJTS1TiEWEBwcdqiUPKQH9QIaVchbUHAgHK6BnyYhEZLoOkxmJSVLkCtJTF+ioaLqglxFks9DwRX4LniuJG/DRAEqZXBcBSWoIRSBbUFIh5JbU6aLFvhIpkcly2aoRGOCcs8h2tqbCFLkweeO0uGl2PXKK+SGDnDW2jM4/fRz+NknbuPbD63nzq07OOwKJotF0g6Y4Sh+MIJml2gIacQCGp0NzUTiJuWKjakJTD2MjoZVrXLKkim4eh3nnb2EFrNKKBQmn5skgErSNjGDEbKlCuMlQTafQQodK5fi2SceJBDvIFEfZufmnew60ItQdYIhA9NQ0BVBQNEJarVfX9cN9GAI0zQJaQZBNcQbr24ibbnEpjQQ62zg9huv5/QrP8s93/oJSqSJ9132Qb59z29QQjYDPXvYvXMbuJAbK5EbSPHyY39mzsqFLLvwEixpsmPD3wk2hqkLR8lbgtGCIJUbRw+HqXoVPM9jMF/l5RGbxojOQs3l1fVJ8tksxUKBQqaE60FE1dnSZ3F4oECqWMQ0FYQhcDEouz6ekAhf4nq1wUQpateAGaJcLZPOl/BDQcy6DlQVEvGayuxqAteWCAMsS6CoPrGIiqJKVHyQIFwfVyqovo8iJS2apC8v2L2zh3PXLKDnrW0kpnSQLVeZOyXMSTMjnHrdr6lPGKzfeBDd6+PrX/00pyxbzmknL5D/e32Lgs7WmFzaHZO6Kv5T1GH/lJyiELUqYXqTJjsMb12yzJpZ7aF1V51ev+6Ng7nb/6NYLhAoisrj93xJPvbMRr73zSt54JEdbDs8wscvXIJW6MXF4D2LDG799QEumhegrUXlhW0pdp5wiUhBTK2lbFs+CAklByI6hAyJriko1OwTui4oVyGiq4xXwfZqAkxIBV2DeARcAcITBFVBSAepCPBq99q2R2MIqo4goEsUauBh6hLLV3AdSd4WxM84m0BMR9MDjEykGR5Ikx0aYPbyyzny6qtYQxn2TgyjBgMEoyq5EjS2xFGESpNeIVW1KWohhmyFbL5C/ZxpVMsWpWSOQDiM47lMFCt86jMfJ6/FOX1BC7GISfeS1VjFPNmyRcrXQAgaIwkUI0Y+k6FvoBcZMPi3T9yGEm9GIciBfbvI5Ktouomp1DIrdRSChgq+gy81NE3FdQWq5+MLgWHoSFXBUHV27T7MsoXzqJSqCARH9m3iys9+hjlnnMzwoV0kkzniTZ1Mnpik//BxxgeHOe2MVbTNn83Urk5Gj/ViOxXymSxOxWZ8JI9l2xRyRbRgAOk4WI6FWy4TUgVI0PEJ+A4Hki4xzeDIZA7NU7DyWTyviiMlWcsnh44WFFg+ZAo+vueBAqZQ0XUFX2goolY1Op5PLKRhuR6DqQIzWusQwRB+MkU67YBQSGfAtwVWRSEcAiS1CtoR6DoEFYnjg+OBpyg065B3IeUIihWfjXtG6Bkq8bnL5hEuTTA6lmLO9Bbe2D/CyQua6Wxs4OHnD3PFyTrfvGczD3zvAu58YOft4P1vAcRIQPDrj01fd9+rkzQp3rpV3ca6obx/u+fXvLv/jHr3P6VPcXpLUEYUQbbkUD+9jT9+ao78/M8O8komy6xWQx4ds8V/ZOWOAMJBncd+/UeSeUGL1cP23VkWzunm+p/uYWZAYeWiVt44nGNBK+xyGzn4Wpnff38O53760LvSjAQEcQ1yLmi+YCIraQgLgoqPjUKuCrYjCGkCofjUhwS+L4kGwPXB1AQTRVCkJKpLpAnZgiBng+v6BIqSRETgaSqRsE6+XMX1fIquSoMvEFJi2dARlzS2dzE5MkT77CmM9U9wYH+SO664mtceepipU2fx0MGtJBSXlQvnMK87xswmKOTTvHi8SkjGCA2Osi9dAFvFCwaIjyfJJdMkgjqoKr4R5MwFnXTN6iA0NkGiJUZRxug5coREvA5fURGOy1DfMUbsTeSKNqFIjFLFxS9YuKaCpoBPiWTJxlBqhmTF0IkHAuDZIFQMM4ymaShSUHU8pJSoRgCEQqlchECIZkXlwUef4drrLuLIvhMEU5OMHO2lZ9ubrPrw1bz+l7/w1H2PcMqa1Zx13mpczWR0okr+wDYKvoaiGBSSBSrlPIqioeoBQmETNa2RnMyg4aLoYfRghK6gx96MR0gVZBwfXVHYlS4TMgLgVYiYGioKYU2lqAlCCY2RoQLVkouqSBRFJaJ6aArkHUBxa3/6HniWDZEgEoPxdJbRTAFFSqZOX4TTu5WwIQnrkmJZQVE8VFegeBBUoaopRIOSoqUQEpJ2XZAsg4ekwRUUEFRdyTUr4I1jAbaMuQxmqjz42928dc/7CN4Dr+5IcvE1Z3D/ffdywwVLuPHC2ay99E7WnDRbbtp+QPxHAVFVJB9eEZef//0JfnBNK1o0wXcfPE6d6chZM+O8eTj3bjLfP9e+mH+iSlEhaAqaTClVAW0Rm/evaaGjDj7/64Pc+dk5DI2msGQCQ3XWZUre/2sLgQQcx+WKJe46x5OsPW0p6x7cxc1Xncx7zp7CDx45zm9uauMr9w5z43kNnLSmiwaqrH8tzcSIRVhAh1bziQUUaNKhKQRCERgaRKMCVwqkW6seG6NQdWv1bUNEkqsIEiGBZ/uUPImmK5g6WNUaxx0EIgFBLAyaInFQCYZMZNXCF4KqB/GQIBAAVUoal65AzOxirPcYVjXIlg1vkwtEmX9gL28pEY5mRpiulLnli7dy9ecvY9/LLzFcrHIw41IhxIn0BIfHVYq+IJkrcv7sdhqqWYTn0xgxyXjQ0zvAHT9bR6pokR/ZT4l6RDGFGaojnU5TTI1S9TQczaTs6FjZPBO9fXR3T+M3P/wdwVgdQVWy8a3t1EWCmCXrpR0AACAASURBVJpBOBIiETQxVQW0MNGAQNeCBEMBNE3DMFRCmgGKwJM+0WCCcDSIppngWvSNZTlp4TxSXoXhEwPMXLSYB35+F4lEgis/fhlmJERvTy9joynSqUlS2Sql9Bi+61OqFtCrLmVXMDE2jEQipUcuX0RRdKRnoeph5oWKrG122D/pkJUQUCSGECR0Sb2mEjc14lEdI6ySd3zSI0Vcy0YzVTTNxMCjK6ghfEHSl+iiNsGDJ0H6BE2dqiuIGirDqQyt8SDBRANxLcjk+GSNW9QFrgdFR2AakMxBIlbjqm0HYgGBLSFvw5SIoC4oGC9LLAQvHK5yyzWd+J5JNQtqNEx3ncOOngzf/d4ajh0eRQlG6N+1l3ltDr3HRhlJTpIu/8ezF5d2hOWslbO4dHkTuw6N8dzOPOuujFLJSfYPW4SFt05o4nbH++eqF//BnKJAiJotxtAkjUEhpy1ezIHHb2TRvBm0zF3B/iM59u2+lwkaKbacwRe+dTvvWdxUGwv5v9Z+4n9e/p0R0XWDoPr/4Cj/FzAKfvSyK2699RpGJsvccGqAG7/3EglFIxIQTGuvZ16jzba+Ejse24wj6ugZ9Qgr4AjIC0HQkFSBpCMZLULJloyXJcmswPZ8UAUFX3Jw3McW4AnBvgmBKqFoS1xF4Eqo+JKKDYYi0RVomwLN9TV+suipJC2P8ckimikIm9BsSFTfx1Bq0zNaPExxtJ9itcz2NzbRNns6Zn6SB9Ug2WoWPaCycsVyZrdXmbrsozx3KMfW/gpjnsaeg330D3uU7SrYsGxBO+coHskqJAIRxssO+dQkj7/5ImP5KunUBFr7GUgnT1Hq9A8M0HfsGMLXSR95BzXVT35wkMmszYEDBxg7MUJFusQbdfYcPEwiaqCpGroQNEZChKIRzHCYhkSYeH0Tza2NGGaAaDRMfSxWy34MqNQlgviUUBQPq1pFUaPkhycYSJeJChU7lyYzNEw8UU/79E52vf4Wzz29nuzoMKnRMaqDg8yb00jn9CaqnoeTLWBZFUqFDIqvUMqWKCTzBM0gjufgeYKy4xHQDCrhBl6/ZTZXdcYZzLmo+OQrVfqLFUZdwXCmQHqshJov0RhWaQhrRA2NOYaHEQiT9n00RQG/FpCh+AIpwPFcKrZDQHcpOy6e63NiIsNbBw6Si8aZ3jmDYl5SyEtQFDzHp+pCPA4nMlAoC6ouuJ5PoQQRVWI7khNJSVUKQu8e8GgZnn9pD88fnODqlQ5bj5e57ROnceWnX+PoiUk2bXyH1auaiUyfRcYLczSjiv+lwv770QKUdwFD/N/O1P+kvPj3u7jrM3PleR86k75xydrr57HwtNO5/bIIX7o/Tyiu8oVLmvjMdSvwhMH0poD895P7j+cc/+FPcOqiafJXX1nNiNJNR5NCXXGU17e/QyIcIZUr0hrTOHPtSfzy3g18/Qvncd33dzFVTvLn13v4xve+zC++/G26FsyiYe4pTFUmyPgh6hoD9I8JvvK5pXz+u1s5u1uyp6/AqYvn8S+33yU+c/175dxlS7lsbSNf/PEBLjxrCk/89G7mnbaKVGGQqaUkVnMnb764G1uojDgGX7x8Dp/75XZmaIKoqIFqTJF473IQQSHxfQWpSOImYEp0T0GRkkhQkiuBIxUagz6eMBDSxQgqZHM+0VBtYVOpLBD4hIOCgqvhSg9F9ak4Cr4EU0hMQ1AsgWZKTAFVWzLzw+8nIx0Gh0cpFkyKxTJjfcNYnkfI0Jna1sRn5kd55GiVEV+SnUyhKjp2tYLt+Fi+R8QIMDie58IFLbw1OMF0M0BVSvxSkqt//EtmtEYYSRYIh+qpuCWkMLErFcqTY6QrLqgGsZCJL1VGTxwjPzbI7O4pbHv7AKG6GE2JOh5+cj2N0RjSd/Glj6FoTGlvJh4JEg0ZeLaLZ3kYuoIaMLF9AP9dmsAlXaxQLNgUSxWEJsBzCZgKU7s7qJaL6IUqoc5WSjkbI2LgKTqaIsEuoiCIRoPkCkUMw8DzVRTKFPIWuWKVeMM0OqbGGRudZCI9gV8pkq+oTNU8ImTojoS4/lOnE+uAj3/wObzOKA2mSlDzcQI6dWkLTVHoyTuUFZWYIikjaAnUJpRGSxVGLR9dMfAl+NTcC0KFhro42XwZ165iKAotkQAaGsunt5MgS2rwGFJRqVg+ZUtStGvu2qwniCiS6c1QsQQpC2JSknJqc/M9RagA85ZECBsat9+yirt/9CqvDCts+tNlTLnoYR6761N85JbfM/D6jXzpsT5uu/RU+l2bl/6yGzToGyswtTVEKl0hEmvgngeeEpdcslaesbCFv244wCXLpnAkJ3AUk6PPP0t4ZjcTkw7V8RMsWDWTG94zmy0b9rL2wqX0DTuctSTGn/66mcZ6jZCicOmFq3n8tSN0hsq8clznzif2/sMx6R/OKTbUB/jsTzaRHHwS37Npbmvl5LNW855YmqWL5/GxH7zA8e9t4+JzFzD37F8Sb2vh7eN9JEWUvu2HyLuwdNVyCtV+SkqIaYYka5doro/ypzseZWnbHEarBZbPbEPmJEIILEcleWgfvzwqmN8eY8/WowTVDCeO97Hz0ADLZjfSUjxBT8pl7eI6TlsyjS1bDwE1/6CGJCR9NKGga5KoAtJXEJqPrkIVgZ0HU/PfbQk1hOIRVyXBujDZTIWypxLzNDy/SrkqkIrE8Wqz0rIEjnQoWLUuSzcF0vMp+4JASZJBpckA6fhE27tpnzGVkf0HaJmxmMqRfo729IIwEG6BkmvRNGMFvzuSRGuYwoJFzezY0EOllMNSwKt6mFIlnS/z3nM6+IBWx5lxk+0TWSZKBaZ96os0xJvRDZNZczpIJVNUCgpDfcOAR6FUIaCZ1DU1Mzk2QSgQwBMaxYpLfbwJQ1cJ6jp9vUcxFA1b+oQ1g4Z4hFhTnFi8jlIyR0tDHC3g43sa1bJNUJdIJNmSi1V2SZdsKpUiKAZqQCMSCSKETTGTg0AMr1wmWy2xgAq9EyPUO03EWpqopCZwFYEmPVzPB9shWSwTVEANmAhfYrshFi1fzmP33oURiqHrOpF4AEPksY06PjK/hXysyrZXDzO9w+TJZy7kD9/ZxqsFFaoVppfKTOgBApZDjws3zQyxZbxISDVIVx3mNkry5dqaAwmoCkhP4gsBjotr2xi6Cr6GJiBdcmgISnYd62d2IsCcmYs5dOAgAU3iaALNg7QNDaZgJCdJ9oKmSSwfFFGjco6XBUGtNls/MGSxrEulOprnpV6Pki8J4tEZUPj9Qy8yvx5WXfZH/uXrH+HBR16kONxHeO40DBr46Kn1bBwxmTpVR/Pa6O4IyTVLFzI80ccHz5hKoQSJuEeDXsdwZxvexABfvukKvv7dMTa91UslnSNd8HjmJxs4MGDzg5sWc94ZM9i65wSPH1VZ9/Cj2C60tYU4bdkMIgGNYtX9780pHulP3276pXVJO0BXI2gJlTmRLL99cA+yeSqrV0xB2AbaxG5GciYXXrCcat9xhsseO/b3oCoG+/YdpZAs0BkSPL9jmN7Dfcw0R3h+a5GV8yIsDaeYGBsmqI3x5sH07deuCa478c5+rnj/GgontvHKk6+Q9E2iMsnAOLz93A18/NubueT8BaiFIgePp4iYGscHKzQrYCoQ1AS+CsKDSVuS92utzGhVUHZqamrEhHBAYPkC1/VxFAPFt5BSEBQeOB4tcYHlAZZEkaCbConmGK2zp9HS1UD7jClU0kWCAYdoRGIEFfA84iGfUF2ElddfwZY9PdRN6cDN5RF9k9zU0UKTbzFqu2Q8g/ap9bTM6aZ10XQyIz2kxixsN0/AcXCloFqtoukKc3WNR/efYFfBQlbL7Ml7nLp6DYmQy1uHJtm4dTNHR3JsebuXWd2NxJunUMnmSY6O0t/Tg8jnGDp8kOxQL8VsilMXL2TX7v2EE0F6jw4ALvFgiEgsQiIWZWpLI62Nku6uOLbj4rsKVr6IZ9sII0gp76AKlYrrUXCrCF/Ddh2wLRxcWhvqcK0KmmEQisWxSnl8ReXUU+bz/Mb96E4Z1/OQ0sKtuJQtG0cN06JrlBwP27bJJnOc/6H38veHHsSIxMCzcRyPfNWma0YnR1NlzlvRQEvIpL2ukcjsEIXG07nwXy7grZ++QMO0GKOeSZtdYmNO8KUZAerrIvQWbQwpaI7rFG2HSUehbPkIVa/JdJ6PqutIX2LZFrFoCHyJ5biETYHjgaEKipUCVrqMLwQjWRddQNisiXSWhDoTWhMghWB6XBA0aoBpqoL6UI3XTkkVRdW57vw4tz8+yrXvm0Nc89g5UKGxOsr0KYIjSYXpjPHZa89i4+4hGBumb2CY9VuHOa25gGqlWbW8i2defodj+3czK+Hy+2eOU8rmCOZHueeZt8lN5BkoCZ56aS8JzaGhpYGOFpOhY5MU8jYvvnALv7z/HQ71jPPwhjFevnMKf3thElsYmHaFV3aPiYoj/7u3z4ImXe5oXXjSiu3rz+WNv7zDeV94mqaQ4LR2QfPsdo6Pllg2NcyO3jI//EgLBwfL7ClMx0oN8aUrTqa/VKY4NMLL+xWefu5t/vzH63CJUbYKJJU53P3VdSxflCAqHHYNacxbtZTOTpUlczp55elNPPNcD2efGuPohORHd9zEQ886/OJn1xOvO5Xn13+L++67n7ffSONUSuSSJbp08H1oMyVCE3iOIGpISi5ENIGu1/jBrAUdEYGqgKH5NcZEAdurKdCaJihVfCpe7cXumN/KovedR8vsZcQiGmY0imFEkVYORQ1BtUTJylLc9zTqtLXo43vpSa9gy/GNdCw+j6MH3mDAFlxbsNmZHsdOTqIJheeH0rTNn01i1hRKKY9oY5T04CRCwkj/EaySS87yWDB7CkYwzvA7B4m11HG8Z5I9A28x8Mz9jJ7oYe4F1yDyaQLt8wgGIdY5jc9f9jGOhbrp0qvYdpmjW7YSDKkIFBqDYS5eu5YHNmygLhZiz+Eh4rqPaoZQhY9T9Djj7PmYpo5mCiLhALalEQmb5CbTlKsVfKlgWR6oPkf60+QyaVxP4DgWvhZgakuYZKqA0IK0t9eRTE0QVDUadJu5py/jnnvepC6kEoqG8F0LU5UQiOKULSIRFccVWHaRmS0x9g84VAyFi84/g8nsBHs3bwdHEG+pY2lc46OnJcg7Cmo+T+5wiuyJMqvv/iTb7nqO9HiVeZcuZeLgIU68OcCsZTGePGyzsFVjNAOWU2HHhE9WqkjXQdVMXN9DVxQ810dRJAFTRygKtm2heRDSJaaioUrJqpYG8pOjWJ5FWBdYno9UBNEIWJakWoWJiiBdlRhK7VBbouaKmPDA9SV1rQk+d67kqDMTvVrk9ItXcf+fDvLtfzuHT1x5J/d/pZmL1g2jKCqP/uVjFPYdYaSkorVP5ce33cenP9jOnX8vMHN2A1/61NmMHO5nxuIObvnyY1RFhK9dFkVKhYZoBDOs8d6v7uWxr04nL0Ls7nXJZco88Wo/L/1bN194aIwLF6p8/5kCpy5o5IU7z+Wpdwy+eOuDpB1fVP/Bwss/DBRndE+TppVhaDgv5s+fLo/1D3Dukgjnzw9w+3N5rj+rgVK6zLLTumhXJV97vJ8vntPOnW85DBw8gdR0YhGNnNCZFjU5c6rHO8er+HFBtujT1ZZA5vO858wOXt1TYVGXwfbdw4yOF4lNbWNaXJBOZdB9h837Knz+moU8nlrCF8+3+Pov36DFrPKDr1/DBZ94nE2/P5+bfrSJ4X2DqIqgThNo+MRVgaGD44Kh1oQRXavZC0wdTA00TxCOSIrFGn/kuYKyJWnsiJLomMLMJctp6GigccZcrMIA4boO4lPm41aSuHYRD5NIVEXzJRWriqZbjI1YlIs+r21+jubus0iOD+EbkvLzb3PqzKmkciWOj44waVV4O1ulrqGOeEsjVjhBpFQhOzlEuL6FSj5HJpXBEz6XX7SSV5/fxmQyi+O4HHv9G7z0y8dI+hJVNcikJnGVEObc0zj9nCXU13VQdXUObtvNg8+9xsH9PSimyhxTJaJrmL7N+845hx9tepvGkMmR3lGiQR3PhVhDgu5ZnTTFIwQD0NaSwHN9Bk+M4TgCx7MIhcJYroMvPbIll96+QVQzilXOUsm7KOEAmqzi2z6ZYoWpszvRfEkukyIej1FI5rngwsU8/+YIycFhAqaOpquoqo+q6Bimil0uEQyadMyYzd6DR/ny177JHd/8VxoaIpiReiynilctUQ2E+ft5kkE3hJMV6HoZt7GdSu8EkWlNxLpn0rByKbprM3r4CXb/5AC7fYWAaZCqQrLqY9k+R0tezX6CwPYkQUPFcnx830ERkmgkQqVUIaCpBPBx0EjoPlHNYHmgwmCuBBICJhQtKDvQ3gBBEzJFiSJUgqZPsqigKJJ3khKJYHvF56nvnMxd6yc5d4nO/iNFVs2L89qWPs5bO4/4svP4wc13EA9KZEAjGvQwwzFKtkZAVlnaZVAiwSt7kqxZ2srE8DAZz2TwRIapLSZDaY/FXUG2DHp4ikbUt6kPCY7mfK695CRWdgc5NDjMpz+wkvV/30xbg8qtvzvOh1fXsaAryLcfHiPWFuNYT0qEhf/4qecuufyZF/YKKf8jxrv/LKD4rh9JKBoTe34hL7nwNm745NmcGChzxeo6fvzQLl7c1c/lS6M8tCnHnIWdvPHQh5nz/j/xh6+cxaO7sly1LMGDT77FR770BdK9bxGOhSgYM4kFbIJeBhE0aTQjOEYAx3UYGximqXM1hf7t6O0xDh9x6Yo3YHbECPs+WvoYuUCc5N4+ikY73/3Oj6liULVVHv7VpRwdjnPW9HE+8LXXqE7kSQhJiwZxXcEWkogGgSBIF6QUeH4t5qlcgcYA6KqkZCuoKggpiRqS6SfPZ8olH8GoVmiZ00Xq4CvEW1sxogupn9KGX0gi9SjSzxEOabi+guUoeJUs6ZJCLlNi17YnKBRNWqctIJ0dpmHx6ez56c+Z39JEulgmVcoyYGv05Qo0x+MokQhaQEd6gnImSTgcJ1fMMTKa4aZrT+cvzx2mmpnkh7d/lqBXpTnfw1+eO4hbzTJkaczWVAr1dVy+tp3YWJ7fHsvz1V/cga3oKMUEx576FUeff4UjwQgzVLAqFld/+HqueuRRWiNhjg9NElCgY94c6sIaCJjR3ko5N0l9LIxv21RFBKuSxiWIbVVQNQXH8zg+VsW3KxSqRdxCCUsNIH0JdgnbVsmVKzS3NdAUCTA0PspZS+fy6Bs7aZCSiy9bQ6qscvidfsrpMiXLwlQrSFSmdHTSObOVgxv307ZgBgc3b+Hy669DjJ3guB+i//BBqraD1AOctSDI55Y3UomWSaw4i9Jrb1IUQSrhEIVDOSJ5i8ZrVlHc+g6TwqTcm6RUTpNxfMaGK+Q86Elb9OYdhKajCA1DdWudg+ehCAUhXRRdw1QFui/RPBehh4gYgtMMm5RdoFCuVYm+J4hHJKmCpOoJIiY4UqArPpYPFgqTZcmJKuQ8yc2XTuXtwx5vPH4abaesR4tEqddTXHb+hewbTvONzy0l2T9OXUM7drKXalziqjG08UlKAZPG6afyte/8kR9961pkuQ8xOcHwkT48EUTTdYhrZIYLxOfOx8kVMepj/OXe5znlQ+/hrKVtfODGJ1n3xTW4o0N8+QcbidWprGhWefG4z5+/soBIKMJ37t1HbEqcmy7u5EfPu7z++nbxX55TFO9+xa/ceIb88XceIKvo/Pzm06hkkuw6lmL24rnMCfQxnCkykIQzZijc/PtJrr90KfmxUTJDx/n2r7cyMV6g0PMKlZ4jfOOeA9x6boDv3/ECMjKT8VGfv7+wn43r32T79iEcIjz2+OtoRpgd2wYI1Hdy/wNPEay6jI4e5dCRAqmUy+5jA5iJBuaH02Rp4fu3ns7NX36KjLR5/9VrePa5/RjZIkEhaAgKNAUaowI9ALoQtaA7T1JxJBqCqq9QHwOh1KpI2xUorqT7/HMx568kEojQOquFoBEk0jaXSLQDxfQpYyFa2yERxTMT2LkMqm/haGEqo31EYmEG+rcyNJTEFR6KpiJjcby9PaxpbiJgKOQtm3IZ9k+kUDUDfB27WsKtlrAqFo5nkUlXqeQkrVGVVMVD5Eoc6nsBu1omJJOohuCqn9/D+TfdxCXXXcqyKz7K8T8/TqyY4Q/7xnnf7ASHXvgL2kiWvz37LBd/4RsUXn2C2bEgtq/QX6iwevXJPLl1NwFDYBtR6lvbKIwP09rRQkiXaIF6Vi5tZ3gox8pzTkIzVDJFk7qAjSugmC/QP2khhUSqKrrvUrYFEc3Bsj08XyCEpOp66KpAk1U0odI6pY6WkEbBFux4Z5ywXWLhwnYaOyKcuWYRRv18Tjt5PooZYstLm6jKKu3ds3BxufUr3yI0sxNtaD+BaCuTkxNIHApjQU6aO8Bo+434j/yQ0Pw5KE0GldEykWkhsrpJecsA0dPm0eAMEa7YJEJ1TFUDTJ8boj4M0wyV45aBZfmoqoamqgSkh6fWZr1tzwdfolJbTKYLUHwfC41oQMWrlCCooiJ5d9kkiZCCrkBHHMYKElUImqIwWZB4iqBOgxEbrrtoPm/sTfPkayN4uTQv/O1Wnv7Dm3SvPYmEqdLTK8gWcrz62hGODOfZvadIwW1jy8F+RsbD7Nl1gvLYcXbs7GPPtjH+vP4dOpecxLEJlaMFnV1vHCSdqnD3Q9uIWf3sf30PT+222Lr1OJ9b28rmXcdomncWbe05tr89yPQ6h7kzY0yJ6gQTLWzYephLTm+lJa7wiR/u4+5PNPH4ptTtnu//1wZFEKw+uUvOb6tnRVuVbcdKPPn6QV7ccozzrriJbYd3Eg11MtHfT0eDIBiOseGp27jg6p+wcfsIfSdylHz46hevY83KLm7+/WH+/KMr+OzXnuHKm69gbpPPrGZJWJVkUsOMlmFJo+TYgf10Ta1Hb6ijmE8ypbUe1xogodVRyA3zs3tf4ty1p+CXxuhoDTN09DgP/WU7K1ZO45pVguJYhi2HxlCyZUI66ACKwHIljlvbo+FKiRaGcFjBDKgIX9aM166gVIWoJpl3/XW0TJ9Gc1cb0UScuGnix4JUnRI9T79MYeMG/L4+Sm9uYOTP95G870nCc7oY7B/ELWQ5fOAgO/buYNeWHYhwA8gq+XyVcF09u945xl5X8PjG7aSjAY6OZfAViSoUbKvKyESelpY6Vi+MYVk63d3NLKz30H04cniSTU9+gD9980dkXn6GQGuYxlVr2fv2Vgb2vsXEZIqHPvdZrvrD/Wx+6zjixH6SwSglLUIxM8DiOVP4+4MP0Sp0RooOKApVx2XGrPk8vW8vmqYjXY/sUC9qtIm1a06iuaWDro4QqXSRKa1RJsdLWJZPtVBAaoKJ4SzFso/w/JrKrgjKjiRo6kjNQPU8PFVDcSWW4+A4Hrrqoes6h46NcNZp8xidSFHX2ERbZwt/f+5NThwbZ+eew3R2SJyKzYaX3yIcMqhUfDo6ZzLYP0Dv1hfYt3M/WrQJRwvgOmXy2QqEfBL6HC694b0cLzZQOr4ftSGG7Q4Qqq8HRcGdyCDGRmg++1SCbVOom13CyoyRCAaZPFZl6Q1L2bcryb7+FNGwiaJIhJBUbA9dgKbruNLFkFrtfVIUPOmhS6gPmbSYFVy3ZuWJhsAwBVVb4joS2xN4suZjzVu1SaqCA7oCvTZcf14Hh9IFrloV5W+b02x4/A0u+tSlVPv7SOFyYPs2tuwb49r3dPPM9kluvKiBPaUg08M+oYZG+ndvYm5dgAuu+yCPPrWZy1c3sGD5bJrqQyya28BDT+zj8kuW0e+YbNmb4dcPr+NPj27Cqrj87ImD9IyWqUz0snlTP9VinrIt2N3v8tUbl/HQxlF2Hi5iFdL8bkOB82bCW0d9rn3vknWv7xy4/b8kKEZjMXRNRfgeP7/15HWhgOA3fx+jfyDN4haHOaediY/DfXc/y83XruLI0WFs22JQtpEpVGmoj/PrO67knQ1vkfIUUid6ePbtcQJeleP9wxRcBTMseXPnEM9uHmHvwWM01tVzxtIYv16/H63qUTI0XMVF90uUija/vXcTq89YwiPrd2LiMHPGXNKFEpbvYGVzfPL6Vdz77FF27x7m4o9+ktzwIAcOjdEkAFcQNiRBvab2BTRZUwOrAs+R+K5ENRUUQ0FVBGZE0HHSKvy2DsJNERrqdOriHpW4z+Cfn6H4yPM0zw4jQi2kJjPYVZs577uK5g9dgtk2hWh9G7niCJ4aIJ9L0jlrMf1jY2RSZTKpIU4cTBFurEerpuhaMJ9oIspk/xC2VMgUqgQaA1x53iIWt4Z4bmuKxnaDyjsT/Pi713PTDz/OZ249mzcfOUZXXStK20Kcoz3s3Jtmx98eZiJnseOFV5k7rR0/vYELv/FtJkPNePt30pXQiJUlP3u2h2/e/W/kBvtowMVRAhi2T2LNWp56+WXqYmFKuSKx9lmsPnMNfjlFcqAPMxCgsT5EMFqHXSiQzrmo2Ci2zfBEgVBAw5FQKDsYporlCuLxKJWyg1QVnKoDqkRzJFnbJmGqlGwfU9fp6R9l9oJFzF64kC0bN5KobyIa0qkL6oyNF9FLZQayVfBtZs+dTyQUZDiZZM7CheiKyp79R2iqE4wlixQKJfAlBwfSnCl62Xx0mEfenOTzP3+SyWGdyaqFt+EQKT1MSVdJOFX644vQN73O0MobUA5uw/zo1WjhBdxwy0mkCxZHDo3heiphXTCzvZHjGYu4JtEwcKSLJsS7bJNE9SUNpk7QrWAYsma/cRQCQUksBImIoOQIGoK1zmSiAEITxE1IVmt+xg+eGuWNl8f57Wsj3H7jqezsGeT9i2ZzyCrQZEqyFuQnMhzLKxiKx/NvDzNrdpShY+P4pk//wSEO5yUbX9vB9798Lt++fw9HRipsfWeMo/2j9PRO8MaeIarZEslclTde200uVyL8rlvjyYc/z+8eRSKcxgAAIABJREFUeI3n7zqXb/yxB89TeOJbC7n+u3toiwru/PmnePm1PTj5KqNE+P7PLwTLIehV1g3m9VGkv9Nzvf/8oCiEINHaLFvrYutWTa1bd7QoLr72PYm2Pzy7n+GRCj/4xQex8hlGBkvk97/J3knJWN8gd93+fn58z0H+9aYV3PJv6wmWRrnjD5tACKKq5P67b2HDX18jpUT44bc+QTmVYppWoakhwYyYT0drgjmtBgeOjPPhMxcwfWqcoFtgTXcCv+zRFFXI5gt0RRTWruiiKWjjSBd/vIfh/nFEOcnGbX08dc81fO+Rw/zyY9MIdc3n90+8RaMhaI2CLwWWDY5TC5aVSi0qTCgS1YVUVRBTJdge+srzWPPhU5jaEaWzvQnHhnLYZPTrd7Lo6gU0dFextx4isuJM5nzoarrOvIoJ2UchP87xgy8TDinEo03opk00bFJxoWt6F9WKj2YoHNk/SKChjsJIEt/2mBw8wUSygqd4fPvzp3DLh+bwk59uYqhiIISHM1Hl5WduYdumLex6+m0CnTOI1is0nn4Os89aSMt7P8l9v/guYaOVUDXDlIDKKVefhGYs4ppLb6LYewxj9ck88Ncd7FditASCHHx9G1f/4CbiK+M0zo2zcGmIZ7dV6Os7jkCgmQEq5TJzuhKEI0Ea6urp6G7Go8a7Tozm0Q2FsUyRUtpDD4Kh62SKtVCFUt5CaCZVoFDMUarYGEGNSsGhWqmgaFB2JfWmQNMkigNDI8NkJodobo7R1GCStyWFvMDUJLNnNGJoGu2L5jMxOoZb8FAjQcbHJsHz0II646PjSMeh4ghURUFRXNzmNrq66lAS0xFvfJ+6riDdbc283JPhw3d8k7knv4d1j21gVUMe1lzFmpUr+Nf7t3Hhigg33PA7CNkc68mSHMxQ9l3iQZOxiQzfubCTBw/kiRoS3/MJaApF2yEeDFJyXLqam7HLVTzPrkkBEhQPLA+qFahUBaGgwAgKpC1ob5AcTSkEpUBpjfGrf70SY95qlrUkCVg5ntpnE3LG0GyH4tgI0xtbWLKsm5OaNGY0CBbNbsEsFOhqDmK6LtPaYqzsbmRaayObd/Qzd0YjYXyWd8aJuVmWNulMnd7GjZd38dJLA9z86XPp8EdpiTjsH5OsX78VuyyZtWwmMpWlfYrOxWumc+9fe0mVHX734Js8e1sLLx81+Zf3NnLp5zaQUCye3uWwelr8YicQXmfZzu2u7fx/0M/+n0i4kRJQaJ/aIi+YGqNguxzJ2AhFIV3ycKp5wnaeGy5uJz5/NZnREe7701Ym8h6XntvFB09v557HNjN1SpS/bcnx029cRV9vD6+8tIu3B+C5O85k27Eqe7fu4sZPv4/7HtmG4uZ5fX+JS1aF2XLCojev0qQrzGySHBv3cFyBqakUHJd6TaAqNpVyLc7J12pJN56iEAoLHAuiQR/LVTANlfGyx/ofX84pH3uUC+pr41qRqCCoCRy/5jH0JEgNdKWW8JPKC6K6T7g+zskfOR8RbyOoOihuFXPWSrSXniKyej7qQD/2/uMEzv80gWmdVErDFIvDDI9PkE8V8FubOLZ5D0cG08R0A08xOWV+K4TDTJQ9tr+6Bz0cZeTAYRRDUMpKkq7FXx/6GE7vJH9/cx8nDpZ56cQQV5+3GDed4SOr2khNjlN2Qkyf3oY0LQg307ZiCQ984W7ev+5jTF+0iEce3cXWe3/F6hXd+EmLE2aUDywKsHTdTURtyf1/PsIzd95PzK0SqotwcXeQvoxJrlLAVAVu+zT+duAdYoqOqvh4aoCT157B8hkNaAGBVAKUxkaoqw/z0uuHufiilfzmNy8Si5nMndVJ7+AYo6MZLNsm9z+oe89oy67yXPOZc8WdT86VS1WlikqlHJGEQEICjIRABCNsE4xxwBjjpuGKa7jY+JoLDm1zbWMcyDnKAoEkFEtSKVeuc6pOPmefs/PeK685+8dS9xg9Rv/o4cvtdv/f++x9xl7rW9/8vvd9Xj+lkLPwW20SlZAvFwmaTfwwIRWKnGNT9QNKroMhE0whMbTASxRSaVKt2bZnM8Ko0KytEftd1ts9wm5MvlzETXts33cxLz3/DGMbhymYimbdoxunpGhUIqm4mrGxYS6sFHj6VIs3vnET20dy7HjDJ/js+97EzoP9XH3Hx/nan72XDVcc5CefeYhf/W9v48Evf48TCwW27qsQ1AO6Ky3+9t4j/N47buQLX36ALQMl3vPa81icX+Urh6s4aYxWIRoDS2b30qZCjg2ugegtY0mQFhSLkMagLeg2X1Y1lKEVQC8SeKliwYNrbjufPTsr3HvfEZ480yLpRBBJin2KtdiiEqe0U8WUAWtx5tRakQY5w6Chs/2vK7KNecES6FQzLDRXX27yb08E9JsmA+MGH7h9A1/4wQI/fC7gwT9weMsXBTIIiELNv337vVz/ji/ht9tcOiWYjwr83fs3cMkHjzFuw96dZSJniA/fAG//3BncgSHK+TyW1JgC9g24dBLNd06t35g2Gvf/X2vN/x86RdPiit2T+o5NJY40Q5a6CZaUJEIjtMLKF+kokwv3bOb8nQXe99GfsqlP86d/+GoWphe48rq93HvfUdY9Qd1P+aPfvoLP/OkPkCas+IJ/+ulZPnL7GB/88iITYomfHfd44pRHHGrW2jGraymFICHxY1rVBBWmFJQiDRJGkhQ/SAl8QUkJjBgKSmDHkkmpEZFG9aDWEjRSzdYBRa+nMLdfxh23nM+zDz2Ha4GIM6gsqSaJMmmOYUsMJ3MVYEpyJRjdtBl78y6spEUcJTj9FeSTP6Rv/zbMTh3rwJsJlaL/hptpLBym2W6zuNwkiH0ePjTH9DMh3fo6F28ZZrDPIV7zWGs0WZtZoldtMXN2nb4Nw7RWmuT6Bvj1338Dv/YrO/nBPz/G4VOnWYxKPPDCHM8+8G1+4w8+wGvedgf13DjaPpeTR89S2exy6kyP1TWff/znB3j3u65lbDLH0nQX6ZqMX3sLR797L2E+x+hoif7BAVa+f5gf1wSDj3yX+ZMNrr79Ct5w5RbM/goD/YLBio3Wgu0i5LBn0us00amm1k64667raK63MQoFkk4L0zRohB5RIMhXhvjSP3+TfRceYH11hSRNOHPyFH4Q0WfnoLECRoJMQlKvQ04qTEsw5SjycUyiDCyd0g0TgiAljUMMIRGmYLCYZ3V+FWWZDFQGqK5U6R8aZsPWHbiGizAUXnOFgaktrJ5dxosyHWqUANokFYowSDALkmo9Jql3MfuLXHbAwi7t5cnHHuC+x9tMDMxz9LTiqR+dZNtlE5gdj/0XDvPVf32B3rKPTELGLZuZFY9cGHLp5edy6YTLl+47yoffdR2f//rjjPSXSFKFUgnF4gCkEUNmyrY8xNIDE+yiQOYg1y9RgUaEYNqQdzRJIkgVlAzJvXXNpz56E2/93e9D7LFZaspKMm5pSCX7HEUxhTgWGHEWcaFSmNSCcSNhQsC4SBmPFRUU40mKkyjyqaK5niIDQRwoFtYUYdTlwedDynnJdM/g9KkAaWcYtRGzwSuuuYz1mZO0IwPDNrj8HItNYwUWOykrS112TFgMjpT46QmbgaJLqlJcSyC1oBXH9Nnw2i2Vt58OxD2drv/x/1lkHeOX2SEKIF/Kc+XOMX3JkM1PFj0CBGkKltAYWtBvG6SJpjI4yDcfWCQIffy1NWa6gt+563yqjYTZ6RfZc/EruP6yQR58cp0Ltk/x4KGjtHzBai+Tvjz29BJxJFjxwIlTdqWKPgMIwUAwKCGHZtQRGCJbeMRK0UoFlZcxGK4B46XMgTKWz+gzRQOKlkCj6DMNqk3YsAEe/slhPvThO/nMvzzK3nyKdLNgddsWuGWBtAUqzqIIMDQKcCyL4VdcRNSqUauuo7DIHT3MrttuJF2aRtVLhCObKOw6l3DlLIvVFer1JipX4tMf/SI3XDDFzTdvQSnF5JBFcz1iZFJw8JwhhJSUhgts3DLMyukqjRDM0OPEA0/x+ENnKW4+n0JUYvnUHN/8/DvotU7w5F/9V9aOP0LcPE5lOM81117Px77xJL1Ts2jT4dIRweiQRFuTvPv3/pZP/fUnOHfnVm56/8f43J9+DBUnHJ/1+dlKnYOHfsLK5p1sHarwyrdcQbfnQZJlNzeXmqRSkCQxsWlytt5FBh6vfsttlMwccwsrIExsndAh4ZFHpjlwYBdf+PvvMjY1weLiMptGR3n8yaeYGikymE8wgBEzwgtjHNOiiEnRkqAkrkjZODXG+68Z55kTy2zJ2RTNhGHHomRalE2T6XqXYsklatRpel2sfBl6VeaWazTaDTrdgE6QYiQtxjdO4HV7BDGkSpHPm+ggws7lEFJijZa45UKb519KEaWAwWGTFx57np5pcujHx7n+pq3c/+g8ug7FKYfqTMB/+uQt/PXnD6EDxalGj/5CnqYXISsGjx86SaGvQN62yY8N4a130IYJKLp+RN41WI0txgsmRtKBVKKFIorAzpn0VlJKlYzmrgXYUjA2KDASxYW/ej2Hf/4kL821GQ8E6x3BQF6zvQSGpdGJoBHCRf0wG8DuPugJyVAR4kRjS816D4YcKDqavCkZyWtme4JuDBOGoJNAn9IcbyqmfIEZwwvzMZaWtBUUHcGJeZ83XNbH5340yw//7Dz+/BuLvOetV3P48eeIE4ujixFnGimPzdhsHswRRyFbSiZaCdqpQmhBM1TMBwl3bynyonLuUX748eR/wnb6l3p8dof6v/6Zg2N3PNvymelompGiHWs25AXzXcVrRx2+s57QZ0InUmzasJWZxWVG2qeZ2LWNV18xzOd+vMTdr7oQo/0in/rKPC898BFuuu0eXDQmil8sZAHzaCiLDLNVermQBbHOYgEQ2KbGQNBKdBYmLsAyMtxXO8rI1ZahMbQEkVGyZQrS1AgEXprh38MUEleycUry9Jzkn776QT7w5v/GBX0+0rSy5CAiDGESeTFBmJIzYKmlufyaPTQGRxCmgds3yvpik7v3RCQD44QPP01469tQ5U1YsksvzvP8o4/ivXSM0/Ue73/3pXzvoePsGLeQKyFPd3xuvWCEJ59ZY2a9xoF9E/hLxxi/5G42Tji447sxnTxxqJGWgx8EJNJleW4e677P89RMk2t3DuNsHOfh793HFa+9nfL2XZjddaobLuCv77qbG1+1n92DkmdfqHPTqzZy6vgKieznh8/N8aEPvZNP/Ke/YqJfQCIJY5+ldsr1t17M6645h7XVJl61gRVF1M426UYRXigRluLZaptHZtv85h+8i/mnnqbrlti7bYJYahZWlmnWDFbPnmZpoYZCcc7IEGdXF+nvExwsKkIVkaRFVhsdpM5TKuapCI+2NumSZ0/DZPIGhwtvOcDdv/Z9+i3YOaBwSxOk3VWe77o4dsBi4lAPA/osjc73YVoCmUjylWGi1gr1IEArgzSJKZZzCAFxHOL7mv6cjZE3aDZ6DGzazafvTvijT7d4z60u7tR+lk7N85X7jjE4UOC/fPQ83vfuhxmcyrGxkEM4Bm9+yz7cksX73vwV7rqwj8/P5giiiNvOG8CJepxDyENhnuJAP3OLDdaaAd0wQACu5eJ5XSYGBriyb4lAprTbYFoa1YXIgIorMAxFpwuuKzA9zZGJnZxcmqe16nP7GKz7moWGYNtQ5qnvBNCfEzgGLHYUm/skHQ+8WDHfFkwWYbICTSWwDc1aAwouGCLTRXYCTSfOolfNGJbCrKJEIssql0LQMbLPUFJx4eXnkCtt4PzcC3zwX1t0j7yHc676Iu+41OTT9/e4/LLrmJ05jmtqpMiYo3vzmpOexDYVnQRypqZiSm4bsXkulnzhsbOCJPqP2CkKBkcH9Jt2juxZ8kOWPE3V15hCQQpSSsqG5rG2pl8qglRTGhji3P3no9OAU2seGyYLBIVNvPHKQT77xUd4/JFF7nzfO/nCp75ItdOjYMGJNUFZC/ICNhiCgpmlmBUt6EWQN8Ejmw96ERQtjauzHz39P2YQJriWpuTAcF5imdn7KiVBLidxLRPHlDimxFACy1GkMdRXEsbNlK994ReMn7eD5lIVM9VoLyLoKkSSkoSaJM2ewGZqkN8wiDG6hSQNaVQ7DBgpw2tVnvjSg7g7LqC3/yrS5gq1TsIzDz5ENWzS6axz06UV/vJbz3LHtRs4dbTGYq/NcN7mx08ts2dHjomhPK31mKt+639j89atOIPbScKYsBsSBh6dZpNT8w0cw6AyOMzp4l5G18/S6PRozc5R3DFKf7PK4aUqF7/rnUxZLQantvLNL/2YA1MlxreU+PrhDgOGwZFWjzt3jvOX/3gvXddhuekTSIeV2MBxXR5++AQ3X74J3zfoNTtorXh+uoESEjNNqLU7NIKEc887j6Xjx+lokw0bJrDzDtX1OvMzTeZPHCPodUkCH1CouEc+rzgw5qJ7bVAabbjs3TlMoWAxPKCYmHA5s9ihJop88suvxH92nvzuDWw5HfLb79rGKz74W+w/6LL3vHFuHUo5M91EG1k+ynoERZFg5ErEYUKrU8W0bMYGh1BhD18IjDCmv1Ig9ALyeYvhXdsRXkgUe/T3lXjocMxF4z0aLZszZ9bZtmOS+35whEv2D7Pi5jDP1FlZC+i4EheDX/zsFJfdfB13vPtavv+zZ3nnVs3DcxrbEKgUbn5dmaMvxcRpyqD2iCwTnWjaYYRl50kNyKuI/pFJOkvrWHnQMYgc5EQWlCaExDA0eUsyS4mK9rhOdLh0XGC40OsJNlZgx1RGci9YsO7BBVtMdo3DTFUzUIQkkZw7AqUCuHmo2OCobKu9fxL8CDSSTpwxAIgFmNBIMpp2AQiUoCjANAQjoxCHmtZindfdeR2f/+EC/nqdWy8b4oHDNV553Qg/flJw/r7tFAYG6XY80qBDgkEsJevdkL6cRRBlY7hAwXqSMuRa9PcV71loBx9P4+Q/VlG0XYfrd0zcUzQ0rSBlyU/IGZn9zS3YeH7IoGOA0vhhyM59F2FaBqePPM/unXsZHhnm9a/ezw+/dR8f+J1b+eRnf8rlBzdzdmmJlY7H+371Cr71s2mSSFAUGgeBNAQ2ijARSJ3N8SxDULIyxHl/ThAmGXZRGxklOyTDK3mpxDYUlsy8ylgCqQ3CyKBPJqRpSjknKRYEOtQYUlFyshiAiZJJuV4jZxikaUqYCEwkXiwwbYFpQOpLxHCFdmGCbqDwF1+iWN7O9oGELdfuwi9NIl/zFsI0xBrI88LPHmY5TlAnjjMxYPPoi7PcefVu7n9shbpXI9T9zK+u8dZXTHFiepn+c36FV7zzvdh+lRQbr9skjGFxdZ2FlmJ5XeGomK/83T/x/R//lO7aHOuF7YwuPEefqdjrmPzdnMGdhTWsZpX3/q//nU1HnyIqjXB0bpXxIOHqzbAgHKzZNb51pseqStmkDEYmJjg7v8TGJGCbI+jXEQcuPofUSUjDGK9Rp9sIEaaFoRJWo4SuMmjV2ygiGkvrGAWHQqmfJw8fo9cKmasuUdIgpaCXJJQKko35ADtI8Q2LUrnCBef0c3zJo9+xKIiYtRPLtOM+/vWFJ5j9yafwemUqO8ZoPrjEwK/fRPfMNKnZj871E2wc4dY9Bj/9RQ1lxKz6WSei0RTKFn47pNb2aXSa9A9U2D5QZHSkiCcshKXpGyzD2hrLaz1yRZM9B/fxnX97is9et4EvHq1RTCw27+rjlQMe33uwyQU372a8t87kuRWOH+lgGgk52+Zb//QgRtnlnfe8ibORyzun1vi7J3ym3JCdgy6OGeH1fPbvG4Kex4m2wEgTCgU3E657Aa1uzP5JSdCKSTTYGrAEhpktXYQpCTVMVQKsNKZpaaJEYBqCySlBsQD1rkDGmq0D2ftkqvjxC5p6IjDQTPZrWonkTF0S+Zr+Eiy2YNMgKENjGpI41RTzGcIsjDUxcF4f1CNBV2da3pbOgtbma4pXXL+B2Z7DF7/xGLddtpFfHFtFdTy27trKH//tS7zxrteio5il+VkmN0wizQLdxioSyZaiwbKfUszZeGFK0QYv1hQNmMq5tC37nrX11sf/Qx2fHcehr6+oE2CsUiKvY6ROSYXAVZAzJYvKpNVsceXBi4gMg3qtjmlKup0OE5MbOXP6LEPlNn/2nvO5/F0/Yu+wYGSyn3vuPoebP3wY3UsoSEmfmYEVfJUVv6LMcnBHDU3RytrEvJ3NVYSZ5eP2ehopNI4hWOxkMaFSAFJTyoMfSgqOg2WEtHoKlWbymnwJokgTC4GlBYkyUGmGaTJEQi82CH1FGGcJfK4URCgcCX3X30oQL9KXCIauuI7Zhw/zpn0uQ5dN8OiPT1C66dfpBPO0T07z0AMvUJZtDLuAk65hpUXifBGZtphd9rli/xB9pSKzywuc+/o/4aJz+4l8SbG/TKuxTrXhM7dUo7Fc55Gvf56T6wFX7Rnjbb9yGebSHJZT4MnTLY4vN2n1Orx1o4051sc/PbXKZB4OujHVZsjXZiPOVuu0DbhweBhXx3RNh37bZNA0eN1tOzh4+3XYIiVqNHng4Rl2b++nMVsl7DRZX/Fo1n1UmBBqCWHIih9QCyyCJMTNuySRx/ieizgzM4ufhkRNj54J5W6PrhSYtknZTdjkxBQrgwwYMJmXOFtcVJjgLmjOvylH3HcVzrl7SFd+zsoXTjJ5cADj/K2ogkPzq48w9Jbr6C31SMMILQSN6Rn6RJ7PfvkFVrTi8dWQkYKFlSswMeAwu1wjTk0QKZWijWVJnFyeoNXEtYos1psUSxXu/PXbOfXzx/j5S3Mc/cAQv//lBokt2HfVBRzQx/juTwPqA2UumlRceZ7L+/9ymcGiSSo0JaWY8QVpJ2H/QImVoZRfKQd8fdrg1y/K4Q4oTs35nK4a7N6iefhIwpm6T80zuXxjHyfOzuG6OTaP5Cm0lxGmxClI8gWbOA1wpCJLhHWITZtKSeOUbXrVOiIBlYqMZhIIhAn9pcxabEto9DKLaicWDBU1SoksT0iakKQIkWKZoEJBmmbjpk7HBK0wDE0UaJYaglYiEErTVAJJlkbZBeYV/O7rR7j0vBF+709fIl/RXHtwF08uRUyO7MAPfdZr6ziOg5vLMTw4xOLZGWaXqxRckwlTsxak2DagJNqwWE8EzU7zsGtaF66urItf1t7ll9IppmmKk3P3SJ0e7fTCvyhV+m4NtUEl51Jrd5lvdlDtDu94420cnV3GtQQL80vMzy/g5PJorRgYHmCpFiLCNtfvNTg10+OafXn+4hsLLNcSCqZkW1HQJzQVNwMyDNlQQjMgNYM5SGLQAiQCFWkQApFoCmaWjKbInqZpmHX8QSoQKSRakmiDditmuABaZTOXNIRuKNAa2l5mKXPMNPNvC4mhNX6SiWylhkgKUJrCUB6nNITtjHDHRz6E4W5hf/gDHny0heUFJFsuIS5L/NnTVJe7rBw7wsBoia2yzXTH4PxNI7ywsIhr2mwZssgVh3jwuWVu/NWPMDkAqeeDEJw69RK91OXQvfdy5P4HeXz6CCiT337dNl5/ywGSTkr/watIAsWeSwYZHJhkY9rkC8d6PH+qwRUlHyc1ebGtOd2FgTDhv/7lb3P3Lft47vETnOikbHMVw5ZCeiGvvOtCVo7NM320htAJIw7UXzpO0KizturTqntEno8WFt1E0Qp84ljgp5pUG0RhiOX2sbp4BpIIkcYowE5iXDtFmJC3M6/3mi+YroZ0rJhN/QKhTLxY0lmMWKpuYvLWSxHzZ+CFJXTQxrBigjN1ZLVG8c63sP4338HojyGGOInQRFRn1tiYVySJZKEbEyRgqJhmN2RyahypfFy7SM4xUUKTtDO02nJnnauvvZIrr9nPcw8e4kcPneCr7xrj9JzH3rLgm3OSzQMWaS/gggsLnH6iysi4gSkNcq2U9V4EYUozgFdPwWsuLbJ3j8Pdl7k0TsXc39GcV45ReQfHhVo9QaOIOwm1xGC567FrywTVVshYyWGt7qG0gRmHGJbAlClGquj4Aj81EDIh7QXoJCWq+6j45XuD7IitTRAJhDHECpo+1H1NEGVxGJEhiE2NWZIIX+EOZoVCGZJUCzAEcSgYGM3AJ4YNhpVl8AyXoJYKymi0K/BSgalh0oKfnPK47eoh1ms+l2w1+cZjdSY376PVaiKEZmlxhbW1NWzTZqCvhBaS0aLDwuwiC80upXIJ0y3QjqGdaKor68K1rBu6PX9vHCX/8bbPSZx+A62/MTXsPl1dXSWfyzM6MsArLjqXa3ZsZPPWjQwPVqg2OjTaHeaWltl2zjksLy4xPj6C1+2wd/tW/vs3nuKWnfDkYoqQDoeOdDANyYGKYFynlCwoS00SQVkLCu7LR3ipUVpiKija4OayDGYvzp4srqsxtSRJNA0tEEowmNdIqSm5BpvGXVIVEWhBpZBFkq7EkmoAOSko2JAzNAUhURJGyhkEoqMEEZqKI6jYgnJOoo1hDr7m1Vzz9tvA62Ed/hyVXIVn5jss9xSjFx+gs7ZGqDX15+dYrJ+l2wmQ0sJG8uLsKqPDg4zainauyNx8lbve+17ylk/RNFlv1Fms1bDyw3z9bz9PQ1ocnpmG6ir7JiwGVpYJwy6P/fRJOrNH2XHrJSzet0RpqsLu687lnQeH2DBa4W+eWOXZuRZDSUgp7HG6G/GGN4zROtPk9ssm2e8Kql5MRVq8/e1XEQZdiBKaKyvYYZfO9CJ1P6BWi8nrhKX1kFZi4CcaP/DRiSaSJn6sQEKcChISoihCKUUSxyhMLhuIyBfypGmMRjBZlLx9/wC/c9MuRgPBvcd9groPs00mdu3hqs/cTvqD72MODmIduAhj7QRBvYfUKVEcIRJN/tKdrD2/gnf8LEEYo8KAJEzpemDoiNXUZL4RUBQS07ZotppURibxoy5ez6fXjQhUwNB4Px/4vf/C7IuP88D9z3JmZpltIzmu2QKNasrOHSW++ZjH9u0VCj2kL20FAAAgAElEQVSfASl5cTHi3J1F4lhzYs7HMgTbthY5b8DivrNw9XYTs2jQaadceLDACw+ssHezSb2XEKWSC7aWWV/3WPI1jVAQC4ted509QYOakAyVcoSlCn12j4GSwjIUOhWsNzTdVrZY7Hgiw4pFgjSCXhPyaITOZGhCazSSridJkNi2zMILlCaJs0LmtzRKQKMBXQ90pJEGJCmUBgTKECAUOspieE0L1pqQCs2YI2gE2Wslgp7SEMM3n1hn03ge14RDqwUu2rkRz49ptdskiWJwsJ9qtU4uZ7BhqJ89mzeybXKIi/eeQ7Gvn/VuQHW9jqU6XLxz6J7ZNe+2TqfX+GXGshr/o6fv//OrmDn+5nev17fsr/CdJ+aYW2nS7XhINFGcsGPrRl48dpK+gT7Ozq2wffskbpJy4UUXcOjwc/RVSiTaZMvWKZ4+08UO25yY9ZFKYAvY5cJwDqIEDDsDsmotSLTCMg3iJNs65/IZPy5MNDoVdMLsh0siQaDBjwQOWTeYCEE5D4Ey6IUmoR+SR1NtQ5xKHKAI5HKZCyKf06wHEpVC24d8XtPsQcGRlKQmTjQ5s8Cr//MHEeUufYUSXjciPv0zFr1x/uwrv+DaN9+ObboYeDRXazz+k8fxUQwULBa7knya0DINLslL1m2J7jZ5wwc/haW7lJ0inTDg8KlFuh3BCz+7l1arhqyv8qPfvwBrcIDeiWlO91IcKRnIaXIbh6l9+0mcveOMn3sesZAsnp5hZXaZV20s86pJhwVPM7ZplLvv2Eu3HkMQMz+/xokTNW6+eTt7rt1N2+/iN3tIARVL0Vxt8PCLq9g6U/eeXInw04z1J5QmkRIv1RSlRKJY85MMnRYndHtdhAaFoBNpXrvdohWENJWJI1LOL0OQRiytrTF51WZuf8tFbL/6Zva/5SaGNvgsfvJz5EYt0pMvIb02asd+dFmD18EaKtCdb5Oa4MgCdhlsG5zIwDUSklqHgqFwlMFykLISKpxAIYSi12qSLxawi8NMnLufq254A27/Bv75z/+E2cUV0kRTMCX1KOTOIc3JrqBUsthhpkxbLuNpgFnK0fRTio6k00np1FpcunuCsXKHS68Z4MYdiuef6zI1mafZNHj8JZM7r+pjodPN4k57Cu3Bo42YcaFYjRwS02FurcGNW0q8WI0Rtsl55RXmvUGGCl1cIci5GXS27guaTYGhNSNFgZFocoUs5CwSkqKZnTErRUG1kd0Pcfhy1+hrMCQy1YQ+WFY2emp0s1iNFEEUk8UppIJESRQCOycplsFxM5TZiC35+UIm+K6Rndj6jKxWmBFMDQu+fqzAnbe+klOnT2NLScML2Ld1knLOxslZxGFCEnm86rJ9/ODnh9CG4NjcEs8dPU2r06PZjbnxgmHeeOXG3zHyhXtOztY+LoT4D1AUhaBgS/7wd96sP3jT2D0btpZ4/UfuZ0suQ+nXvZCyKRgb7Ke3VuWt738Hv3jwGdqRz6npZT7xsQ/z6NPPUcw72JZD4PcoVvpYqkUM5v2Xk/MEJQtipclrKOQ1lpQ0fGhGECQZjixngynBVJCzYbAsULGgi6aQEwyXBDYaoSUDJUF/BUwBvUSgpUFBarSKM3inlV04wxXor2jSJBs0Sw2j/SaeklimJDUdSFLyDlhWVmjFwATjl25g2/bLMctjeIf+kSs37OHX/uHnXL9jlN7iGn5/HsNyUcunePCZE5RKJRxTozQ0tGLzQI7ZeoOxXXvYfcNbMbvz5O2E586usXCmSiLytKZP8/BzL7Kts8bbrt/IqlUkf/gUP1z0aRSH6ASKmu8hW5p22GFocCN4q9R+/iOOLQsGKzDbiCkN2xRzmrHNA5DLIdda6LOrnJldQQzmKOdjup0UQ2WLlLDdpnF2mYgsga7WilnpahKtaAQQJCmxjugTgiHHJEliAgzCVBPGKUGi8BNNmgqCNMXULm983W6Wzqwz54dszpn05XMUCzZ23iZdb7J47AzWwhHCpZO0j0yTt2ysiw5gbtyNLvqoKCZSeayhQRLPw4h82tNtnO19yFAxM6cIZQ9f2AxOlXAVRGGb51suMg1oqEyOZUmJ3/XpNtdZPXGaM089ysqxFzBcC9d2EEohDEkvMBiagvMmXJqxywYr4MR8zKCtSAoGJVvQbYes+Cmrs21ufdeNrBydoXbWZ9032bXRJIxM7DShnIsQSYxparqeJtImQZSQTxQvNTXtRBNqC1TMZjulXLQ5ttymJ0fZVQkoyRAjya79xANPQJIKio5AGFAuCLTK0iGLVubXr3uw0gMTTRxnMW6G0qhUYCRQGdLkypI40ay2BJ0eOEJm90vv5Xm8EMhUEfQ03Q5YebALEKUGna5gwNa0omyc5UhYjbP55RyatdjlussvZr22Rpom5EtFgl6LCw8c5LzzDvCN7/0I14APffB9PHj/E2yb6OfUwirHzyzQDSK2l7NG6ZFjdc7dPsIbL9vBhbuG73nx9NI9nUB9/H90VWL8++uhxEBz4c4hfdWejTTlOm/6vXvZmEvZu7XEK/dWODTjsdDsoVXAjo0T/O3ffYm/+NOPce/9j1IpFbj3oV/QbrbIFUuMDPVRXa2yXF3l0/d8jH/5t0OUw5B+keAITZ8tcEyylLUIDJ0tNgqmoJjPhsWuA80YdGLQDRW2C60uGCboUOOFglIJigXwewIlJI6tObGW0vITRgoS14AwEuSc7O+2exLLkrimJsUisS10GNGOBW6qSLQiScC1Ib91P6/6s49yzsRO4qRLGidYT/8j0yrPZ7/1LJ/81auZr3dRpTy58WFCL+XY82ewrRQfC2EJxvJGdoMWS1Qmz2XTplFWqmusdwrUGy3iTo8jL55g9tjzVLwaowMW7plllvIKN47x622W/JggiLlq7xTPn55ncsjGrM4RrVR5/EyDxK9R0JKyKQg6ASuxQWtmke6ZFeaayzxe1ZzxEiJHsikISeOUoBPgLTWYnu1xthFQDzVLzZjVyKDnxTSCNPMMmwbDpiSSkhUfGlFMN0zpJClKacIkJUwl2DZBqtk6XOLaN97AyjNHCKXFiAumFAy4JkgQeYtiURK7FkkRBA4itXHKGu10EH6O3nOzJJEi9E2ckX6S5hppaNOrxdz/+Az1XofN50gSPyBo9Fizchgdn44QVDsaW0h8oYlVgrIsbFMwNWiRmJLYypPGGksoUl6WnFjQraXsGTfoKZPULLB4tkb/ZIG4kbloKkLj9iKO1hU3vuEgOZ2j1a2xbZNN2Etp1RWVHIjEoxvE9KIYP5ZEXkLDNzldS2nEgnakSVWCZdsUi1MUWnOYQrPLbVMZ34AXS2pnPFIP7Dws1AVCw0BeEStJkGS7ldDPyE7djmahBe1YYJPJbvLFLPUvSgXlPkWvKVFo1Mu6wI2DgrwLvTADJRedzGYoVEaW90IoGII0ACMEqTQ5UxMGgiFHsqOYBax1E0FlcoR955/PLa+8ni997Rts3TSFaZhUyhV+8dijvHTsBBvHh9m7eyd9OuXI8aMsNTo8duQUPT9CStg84jDcbzDuwtceWyKMOniJzzuv38J3Hl/6/6Aoiox3LtFUctxww4Gpt1+6x+CujzzKRcOa3eeMcddV/XzvUI22r7hoa4EnplsULdi5bRthvU3XD7DzNpado69cQcUBC/PzXHDx5fzzX/0J3/7u97h4/26ma8vkVuqM5jOqddkVVPIaS2fLj4lBKBQhSTQzNUnehOE+Gy/VOLZJom1cUvIumEYGeo0jQeBrjLxBkICdyzZu7stCcGFmF0cQa+IQ3JzEizRRLHAN0HGEEJKypWiGmnIOSCEMITc6zJ5LDtLtVCmVxph+8mcMJjF//1SDbQNlrrhgO6dOnqUap4jCBprLNU4dPUbOtVCpJgpD3FwOTEG0lrLh4NUUjRbTZ5vMnV3BNmyWaglnjz3DVtWlJw3O9kxUn8OTJyKeX23QTgS9UDFWMJmebbKjT2DYOWoJaKFpm3n6ci5pIrFIOdsKaazXqCcOhxZ7lPoq6DjkRA1+48pBFqshnU7AyXrIiUZMJ0zpJppqB1a7CVGQEqYaC02YatI0YTVMqAYpQZIgNHSjmG4sCaMIZRhIqdFhBJbJtokxrnvTa2h7Lez5M0SpZsvm3aThKqWyQ6/qMTCSQ4xKrNjDLU9hT47BwhpyLsK4wKDxsyp2TmC7LRYfnoGCgfJjGn4f2ystOnFKruQQNWJsA+an23x/WbPWTMlbksTQJEpgu4LxYp49A7BvQHH9ZodXTSnORgarbUXRkcRK4RQLbB4bYHa1SUUELNQ9ekmWrLdquCzWA2baipoHK6st7nj9AfL9Fs1TSxxZSPAbASsthddaISZmtZsiAk21q3E9qAawEBnUvIRQCpCSvKGgtU5Oh4xWsuNyGtcoFyMGBhWWK6i2MrXERFkTxwLH0jhCIhVoW+MHkAhQqSRnaHJONoYKQ4kWgrKrMKTAcrJIjXaY5fmGKgvZKtqCfAW0kblWcoXstOWFMFfPsqeFAblctqisFARKg21qRvKSjg/j117Jlfv30F92+fNP/jH/8NVvIxIPpWBsqI9KqUC73eFDd72Bb/3kpygFL8zMslTzGCubuKbJ6nrIu2/bRbkC5w8qvvp4nTCGazbAozP+PY5Kcr4S9/+/2imaaLaMuFqH6dtfd904d3/6GcxUceN5FfoHHFKtGStI1lsBbz2Q56HpgNPVNpWc5OiJ07zmtbfype/8kM0bJmjWG7z1zXfysU/+MSXt883vfY9N46MUlMfG8y7m5P2/wLCzolXJCyyVBcsX3Mwo3g4NXFMiTU0uZxOjMFNFO9TkRJJhliLIOxBFgqFcZsGLUsjZmjjNHC4tD4TKgA62yJ6GQmTbubyjESpbMduGoBcK5MsPhxBBzsyOzwMjQ0xcfIDJ8Y3MdRW9B79Jrljm419+gvfcuJlue5pmLcdiajFYiJiZnqFbrxNr0ErhWiamaZFEKZWd17FzKkbYgiTJ0aq1WFpco6cNguUTjDiSy4ZyVAomc4HmVNVjaSmkE6dM9juM5W2OrcWYtqQVBDhIqklMWWvqkWa95tNMJWGvSyswWO0E+IngWDXgmst2ctmEID/pEKwmqE6HauTQ6MUstUIaiaDrBfTiBFdDLYF2GBFphZckKGEQh1lR7KWSVDoESQRKY6LRQmGZFn4YccE5I+y+8hKGNk1ibdjEWG6MpWcfotBfwLQMBvokCSZmV5Ab20OupFCpIpqSBJ0eZuRQvHIr0gnwn1mmG/fw+88has6ilUUvaOAHilEroouBry2kbzHjCYIoppQzMGxYaafZ95MxBUPQTl1Sw6Ydwu6xIvOeoGQaJIUSliXodpoMmg5P1GLKokPesKh5HuvrPWSaEsWQ6JioCLvTlLabx2wtEfR6NDoxw0WPhpbEvo0WEIQpCz2HeU9SNCP29Rk8XEsRSmDbko0Vg5xSyJ7PdEPQ9jQVsmOq3xE0OpogycTU8uXZeqozP6u0QClBzgFbCBIgTiHVgiASaCMrBKkSRJGgE0EnEhgGREG2O9Aqi00VPrgCCgLMBAw0HQTNLvS7ZA2IA32ORKXZnDOPJEbhF/r4zd//LXrVZbww5tChx/nQ+9/NbXfcRW36KM+fmMO1JOPjk7x48jjt9QbPTS8ws9pAaLh0Zx+mW+S2CwzyAyMQB8RumVHZ4qGTPrfdcYClIzUcV125qS+9Z7krPv7vkenI/6cvtGybjWMjulx2ee+rp/Su7SO89x37eOJYk6JIedslgi8/3WU0B6tVjzPtkPm2YPvmHDfuHyBWmkNH5xkbGeRr//pVHnniAfxGk29999u89nWvY+bJh3np+EnGhgZZPjtNNDzF33/0k5i2xNYCKTTdQOHpDPff8wRtT2BJCGINscTQMbEfY0gYzCm8GJKXu8pWD7qJ5khDUA8FMtWkCrQpKdqSzYVMiJrEmkZPU/cycKxMsrlLqjRCQpgIBBotNW4hcxNYgCM1S0aZoclN1BvrrJ1+HiP2aCXZnFOaHo12ib6CpLa2wIvPn8VyCwgMQKPIZkktP6DTsbjlFS4D44PU25L6+gpeFFGtLrM29wJWqmlECcdaHhtLHheWUz5ytc17Xn8et17Uz5Yxl+dXPCyRMtuIWaylNJo+a+uSE/UA1RM0OxHzK2vMtCxe6KTUEslc22Mgr4lba+y+fC/t+Q4936OuHdZabXpRStMwaXgBXW3iJykLaYqfRPS0opsIuomm4cW005QeJmGSEQbKQlCwbSwDbC2RKsEwDPJuCRXEdNsx0YkGzUe+gzUxDElM0u5Rbxgklok7UkLUT+H2j9G/91YssQmjPEJUzNN55CjLT66wFvgEWNz/81N4cYVO7SxP1yWTZYtvHFP0lkLq812eXG8SqwhDafJOymBfjjSOEJGm58NMI6LlB5xthJzuwqGlgEkn5sRqiw0FMNp19lx7MycTm1GrxDPVPDPtgHVP46WSxVqX00vrtLsxxTimL6ewGmfRSnLR1jIjrstjpzTPLtqs9TTNluZEy2bBF1SjlEPtAv9wNiVvSvIFA8cImShbWFqyGmm2lDR7R8lkLDXwE/ATgaUESaoZ7BOUixonp1GJQobZgTJQmsjNHvKVvEamGSfAJHPKpQn0QoWMYSwPOZ2hyTohOI5GA6WyRlqQOtmyMVECQsGQkxXKXjfLIap3NEkMBanx0swoMdxq8pt/8Ck27tlNY3mBkdFhfnLvvQyYCX/0iT/mM5/8KGYY8L/8wXuYOT1LrdPj1GqDgxsK9JUc9mzO89qL+7jvBJyZPk2KwwunVplpCj58vc3b//BBPvGfX818V/K6K/Yw1lfQk6ND+t/ZKQqkEPT1VW6Io3Dm/y4r5twtU3rPiEWMfc9zC3BiyeOZZ+ocn69zwYjm+0dhqCh43XkmH/9+neXVkNdfuxXlCH7w5Dq/f3mJR+ZDlqttLtu1kRce+gV//7V/wQpqfOdL/0Jq2YSNFt1Olyvf9mt88Ffezg2DEtOBARdKBUmaQt4SoASlfCYiTdHkXEBpxMuuiFZPI5CkZMJrhwy6mURQsiBvQSqh4wvyOUXbBz/I/mmVQMnJYBE1H6TOtIdCCoIemYCVLFCn3RU0fYUhILfrcn7jY+/A60VoNGvPPkba9PEFHDqxyKVbhukGgunZZZYCC2kZhL4HoUcUvYwe0wrLsBndu5ttO/bQjaDaSKkFIctHT+HHCTrWhD0PX1sINKd7sK5gPYhRrdOsepLn5mIu3BSRaptqOyRMDGqRIo5jgiBl1Y/paYGHxYofUZIG860Ov337Zbznw2+gZDT54ZcPU67082y1SxolrEeSRpxS6wVggpdECMsk1gKFQCpFIgXJyzgnpRLsl+VRRcvFNCFOE4RpZDMypwRGnsE+uOS1NzH/b4+y79f2sfLAo4yOjjC0ZZzY7aMymSNZXiNqakShgHd8Bt/oYrljRF4VKVyUiEnOLhJrQULEhmHNC2sx7dSiXW9RKPbz+GKDp9sGK4Fizk9JovR/J+09wyyryvTv31o7nHxOncpVXdXVmaYT3U1DQwtDEkSigAIGRsw6qOCMOgaCgooZZRxlVFAQRFAJQpNzappuOseq7q7uyuHkvON6P+ySwZn/69/rfT+c+lDXderUtffZaz3ree77dxPSBW1NPgcydTLlYErtKg+hTKoNj6rlU642yFY9MraH7vgk003EzSgxUzGcy3HSrDZeGMpSqXnsK7nsmahTl1GOOf4Y3t2r87mvnsWO5/cR6mhCWHX6d41Tr8CCBa1UyzZbCw5TVpSsJaj4Oq4NFj4KhTAVmnI5uifF7LktbB+wWdVcQupBdrTQtaBPqwQIcIUipkmE5uPqEuGD62nUVFD/KEcScnyEJkBJ4q2SfF4RTQgiEVBSEIsICAW22YobHH+bI4pSI3CIZaqStnZJc0IRj0PVg2pDYEoFmiBkCGwnaCspFRy1U0lBtRpYDdc0itz66j6+/q1ree2hh1ixZCF7Dh6kcbifdReczyUXn8/Hz3svzelmntt1EE2DBZ0mszviNKo1Tl6zgOd3TjN0qMDqY1o5ZvESHnn2EFM1aI3q3Hz3IVqb07xyoExPS5T5zRGmXLHUqjX++L8KPV0nmUjMa1hW/m9PwgTTp1nNSbX3gZ9z3T2PE6lXyGQn2HNgmIlMnikZ/eMp7SabMmXihomm+aTiCUxNw0gsZM/YOK0tJRY3N7jtRYeXf3Isp31pJ7sOZ/m3C5dyza8P8+539/CDVwYYrdSJmCZqepxywSL/xnOsW30MDz7xLMuPOZYN+Spfesc7WZOQaKaiWRNUqj51X6ALaCgoWYpcQ2HokphUNIQAofAkCDcQaTu+wAgHSKWICVJTgSSAoD8pfYFmKLZOwkEH1v3TStYet5ClR8/BNAPxY67YYOuOQZ55fAOVw6OcGIacJQhrirINSVPR1yqYLklWLJBk65Im3aVsWVheAzSBroOFQWOyxo7RPMcu6GOgf5xqWUNpFpqm4+AhFXiagdeo0ZRop6FFmczk8XyH4mQOq9zAcizcYhbQqfkuExWH5bMlh8c1ijET5baSaXhIx2HXYAiER1TTqXhB39VWPiFdw7N90rpP2XVpkgYZ12VFT5rVqyPsePVJjMECi2c3M2w5NBkhpqwGlnRxXUg0hVnYEqNRKHGgGkAIfE/i+i7S8xC+hiYgEY7QagQHkQaBjkMpH+UKNGHgODZGqoWGJSgc2k/3Ccsp7B8h1tyEpiscM0Rzu0FtOsNILU46plBTZfA1kiOTqFke8b7lKGGTzVapJrrAd3AbNqMlC8t22TORIZpI88RggTM6kvx2X5aSEUIpaI8IIoZARgxGsiB1l4jyaAuHKXo2rtCpeT55R6JJH03T8MMmI0dGWXniUu57dgPHr11A6qwV/Ndn3k1TUwIpNSp1j9e3DPLMk6/w28kiO361mXPWJjEIY86bzaIlvcHgY6pK7/I2ThsZ57bXbMYbLk1Gg4amKHuBjTWmPIgKupvjIE0QPlMVCEdNJgo26eiMRtgOJrtxQ0PowQZeqyo8aWJGFElNYBo+jgP5vE8qoag1wM1BLKSwqlB2gnOjkoHmUdMVUU3g6YGHNqZD0YOFaR+nJKhUBTVN4dQUJgqpCdJR8ExFvRLYak0NXEuBLQjrPn5DMu4Ljp44xHvOfj8Pv/ICz932PebPn8vRy1ew/qYbWX3hxfR0t7F9Ok9XQidnK5K6Yrpcw4v4bNm2DV+EmTsrTNecBQxu3kwjHqaa6qWjrYljUhP4UiCEQKIIKY/jOpPve8NV83yrdmju/HmqvSlBT+9s+ub20p5u47Nf+rr43za/mZS9vqSpnr3xX3h0MIvCpKO1mbGKxRtPP0ZYd+kvujhKENcUUkgyrkbVapBo7iSVTHDo8GE+eJzg1geGaA0rSi48+ZNzSZgphnZv5No/jrG8xWR/I8SlSxdw5gXnYKY7ODwwQGzWHB7ac5gHb/o+R8ck6RA03EBO4xtB6HxMKpQWaKdMXRAxwBIKpwHFmiCqB7uVp0CTQW6F5Sla4oKQprAtKDYE6aji9bxi0anL+ORHT2Hd6nnEIhEULr7rBt8OAVIzEXoYjChD04of3/ooxT/9nmgocAhEwgrlGXzkl18gm6lhqQTRli5iZoiXf/dfRIwkibTOzXdupKs5zhnL2nho6yBRI0S+5kMsgrRtGjWbuh9YE2OGz9J1F7D6uD4yNY+x/oNMT2WYODxNKTdMTBq0hss0GxFqRpSygOnxKrrhIZR4yzAfVjpyRkPWX6jTm9Coen5QCZsKHZ2wL6gbIZTj8sUL+3BzRSK6x3TdZLpYoVLXwYCDk2U6+tpZ1JOiO2Xg1xX7MzV27xpk3JYU6ja+8pHKQJeClohBUvPRNEnVDZruNcvFVh66ECg0NMMguugk7PIEv7jlQ5SrLqJyiP7f30uirYd0OIxxTC8118Fev51c0kWveCS7WmlZNJehchNi22vky0V0zUSLSkKaxG141CyF1tGOammlXKzjGiYTU1NotQaPvDnCaKHCWfNa0WMCM6n44xaXVEhDKIeEVIR9FUyeXYWtgn6yKyXZQoWWjgRfufoMLj57JS0tTXiOzTOv9LN73wj7DmdIJ6MsXtDDae9Yhh6Kc3C0yI4DFfa88CCfPrsPP2PhVRxybhlpWYBOYyrPM5srPFDUSRk+iYjA9wXhiEKXPp94/wp+8qtpkkmbxeIIMQNicUG54GHVPWwrYA6G8LHdoG9Y9QmiCzRB3VXoXlAYuBJMXVBvQM2a8T6j0GdOWFIG75FuEHwlhQggEzoMVQRruhTZiiKZhFwxgCxLoC2hMJqCa+bWFcIVGFKgJ9Mk9CKNhku1BLmywPEE/SWfXdLkL0/cz6Hnn6EZxWmf+xQ//vTnSYU1Pnv3syxqDxMXis9etBBb6ggNbnvyMK31AjumBSedsZytey2OXb2K119/kdZYCNdX9Iah4rmUPUlIeKxrjbDFTnHM6lXEY2HGJrJEIga5TIE/PPSI+J+wWh1m1O0CjpRscfb1t6orT1tLU+88nt60ETfcQqso83peQ84gu3zPoyrFzMS0wdrjT2J4+CBnnnoGO6bytHaUscsVTGyuvX0LZy3S6GoK8eZInU+f3MboxgliIZNX73uYD33/3yjssfjT/gH+cNMt9MUCu1BIV8GKr4uZ8CKFowdTs3BI4AlF0QXDC/p8s5KQrwuqDqRCioIblP09UYHSFNlCAKjwDMXDOfjmdy7nU5cfT7ncQPg2/YcyDI3mOHA4i+14+J4ilQzRNytNb1eK9rYU37juSlb8+g9c1OlT9yGidK786WdpCzu88fJ21lzzLaoHNuBYFo0lp5Pc/zTTufm0tRmcsqiHF7fvpUkPM1hRpISNV5XkrQZIheNraEJgqQjzZzdTKDVQWpz47JVUSi9gWSVAx3JqFEUYzYR6uUZzxKa1I8y+nEfUEPhaiJ6oojuq8UADlEIAACAASURBVI7eGj1NPpPTCX6+34Wqg2YoZDROT9JAhnRMQ7LAq3Jo7wQyJMg4DlalTNWWNMUErusypz3G3LiOZ9kMjjocKpTJZgpkKg4lVyCUQhMaIanQ9YCjaOth6paHp1wsK/DBayoYsoR0SdUX6IUjhMJthIRLsTgIYVBlxVhxFH3ePOa2JGhLpxjYdBjT8+iaHUU1RTmwcYK5jech3szs1iiNdBqzpYX9D2+l3hoNJtHlKTS3hNmWILFkGcdGFhAy4fNfjfP4+mEefOZlCrUMZs4gEoqhpIdruRQI4Qkf3ddQjgd4eL5Hya7zztOP4r++eynxqIGhm7yxZYCrr3+QzduGcBWsbJLUfcXP6j5CN7jwrKVcd/WZfO7Kszj93j8ScSxc4SNSEfbumCAsdApWmVDG5oLPncLnQxlu+M0eLEunIX3yNTh2Xpq7fj+ObVXxPIO6EYViCVwICUWpAoWKIh2FgiNIRhWO8tE0QaUOId1HCEHRho6UwKkFTqxUCLpagom0ZwsKNUXChKojSOhgJgSOrfD9YKgZCkmKmsKJ6kSkYrKq0KQiGRKEJDRsH78eLKroJhouXd0mw1MFIjFF3YV0XNAqYVfOp0OTxH2Hdae+h9efuIstjz7F1CtPk/ZsHt81RsqEBW0mU4UaD24Y4+SVrbR193JgYBt2TwKRMom3ruFdZyUYPLyTtcedzM4tz6K0GGEBQzWfqAFKkzw7UWOOUWI4NxfncIZoKMYbmwbY3T8g/npS/l/HZzXzQwo4UPLF02/uUXOPjOBFkoyUDtMXMYnpiqm6R2/CpN4I2GaeVaNv/lEMDw2Qy00xNT5MsqmJZctXsWvvfsJOlhe2jPPtz36YkSPjCA6yZ7hAWPe59pFN/PjCtex5fis7Q2le+uo36DQkUUeRd6FiKZIm1O0gW7mhAsJH3QZdKHxX4LqKqWoAsA0LQTqhCFuCUBgidqBb9LzgeBANK440BHsqivd9+GQuOXMplYpFPJHi53e9wJdvfAjX8dBRaICjAvlCKKTT05nmA+85hmu/uhhl6AzkLIo1SM5NsX7DINb+jfRXkjz2lRvoaU0SjegcrofxttXQmg+yd0KxY/8m1sxvZdpWdKYSmKZJPlsiZQpcpYOSFF0Fhs76F18lFonQCKWoZibAVwyOjOP6zgwaXtLnxxBKQ4/FyE41iGouw5MNaqrEm8AcU7J9KkahWifiVpgVhokajDck5y8JcXpbhKzlsH+yTiYMqZikXnDIliXxqKSryWRyqkZnd5y2lhYmanXcWoODk2VsyyNfaVByZ6xBQmK5ASTAcV0GM1VslUcD6vz3106fmex5M6+ol8IuHOH2x/rIDryIc6TMrv0TRIDZoz7N9RqmFIQrNguETzgtseQsVp7bx5/u2kVudApScbR4gmZnL+dfvobcyDhTQ3mGLB3ZOp+MJ8k+fQAlwZCKY5bP4oKLlpHwKqx+x0Juf/IgT932IK2xONlqBUXlrYejXdeY3RLhYK7KZZes5YarzyIa1jCMME+/eoDLP/Erlva0M2/ObHqbIuQqDQyh8Q41xrilcf8j20FI7rnvndTKNne8NIj0dHI7DpNqjzE+XUNTLn44QvR3j5J1NZYn02yuucRjPi0Jl3Fb58BQjtkdJsp1mUo1s1BziGo19o8oWlICva4wooK0GdjySpbEbih8CXsKUKoqDAn7c8EwRdOCibTlQEgH5SlcN7CuLogLisC+waBQUn7wN5E+WggOjTpUXbBnBhIzdmk6NFg1S+J4gkTMxRaC8SmfVDyM8OokYoKxaSgWBCEDYmGfmis4XQred+EneWPHQ/zsmptoj+s8su0wEqhXLfaPueSdBjd/6UQefXE3tiZJdi+mIxLj8KEBqrUCMSNKZ2srNQvSEZeMq1PxFR0RyXTVo8XU6EzoPLNpM8mwQa5YYmI6/9aR+X9OqP9fVY7ndEXUEaOJf18Y5c5xi7wd2OekEISkT93xsTSTlStXMzU9zvhklkq5Qrq5iaZ0C50d7eztP8TBwSP87qsn8ebWfl7b0M9ADq57Vw+PDxZpTS1gxYI5fOf2B5knYVYokNY4QmD64MhgQmzoAUQzFVVEQ8G/XXQUhXwwELFn5DKagkg4ODqXqoKmOERCgbZyNOeztwZbK4rf/ceHuOTdx2A7DtNWF8es/Airk5JECDRNMVkNxOFVFCUfJhvg6Tp//NVn+OW9Wzm4Zzfd8zpY0NfK5ecfR9uceUjlE42nuePXD7Nxyz5yhSrdHQm++rlzaenuItS0kJPWXcHxLQb7bYPmpEZhLENMN6i7DpaS5BpQajR47v7P0tQ5m3Aowt33PMX+gUHOPmMFi486ClekuOHmX7NvxyDSDuxkWdsh3hznqitP4txLziKfsbn5x/exZfMe1p0wn09+6FQ65/YhpEG1qnPlP3+d0xM2BTuE5QkWpgWlQoNpz6AjonA0n/05xayUSVcqRDgWZt9oiUypTmrFIp5/YjO98TCuHwyHGr4kJAUePgfzVe687SOccMoJmMJBENgA645EKYVSoBsaIc1HeQ5CQqnRzM03386i+Une995T8WQYX2hUMhbf/8/7yY5PkyiU+cjnLuQ/73mBU06cywlrVzBvQQdjI1niEYOJAjz11Db2P/c851x8FqGQweDufu554yAfvXwtV1x2GpaR5vY7N5AdHuCmq85iaLoY4Lek4gvffoiu7hhf/szZdPb1URNNfOrj36Z8+DDhnnZ+88PL6elqAiEZyWp8+V++S1rEic7p44zjutCRXPfzF+hMJOg6ZhXbXriPXF2jLkJ892vnc+n738PI+ARKGoSkzejwNFIKomGdVCKCFo3hmW18/au/ZWrfm7TGFammCJ2tKe57fIw5nSkaToUly9vRB4+g1ytYx57I1HCR+Yf3osIBXNZVilJZUG347GzoNK1cwEcvXcvJp6zB8SWaqWEK962HX/31pKiF+eZPXubwHXexBY31j36Frp7Z6ARWFN8XNJz/XjaMkMQQHgKF0ENs3gN//PgnSYTB1CRSUyRNhUeCjnSDRsPBc6HmCZJxSIYNDo361C2PgTHFvlCUL179fu64/QH0TIHlZ/axYdsk+Uqd6Qp87gOrueXebVx62fsZGx9ECsnBQ4fQpCQRjzKvby6GW2d7/yE6Qx4lCzRdwxcBgb8t7BEhxOuTFSZzBVF3vL8jOfw/Opolj43XxZmLm9WLJYe6EnjKR9cC3R5KUFA6slxmZHQcdEFuOktLRxsNV2HXK+zcleGsM07HbuT4zK07mBPL8Ynzutm1x+LefVVe2lsmylYee3Er60KSaAjydUXBDvSDuhNQsGe3ihmah6JYDxbBWkVh+YJUQjGZA10L+j5K+FgNAZoMpmFC4FUVuuFTcWG6AbqpE4uE3iqZ0+l2muMxwnYV0xcYpmB10idXF0gpyPqBvS9jufzwtsc557x3M/b0K9Qci/u2DXPo8DQP3PZhQPLgC0fYtjfLnJ4OSlaZh554k0VzWrj+6jOJRcp8+MrLefr++4l6DUpZDcMwcHzIKkiiYSiH5Ud30doUZk53knOvuJU391bITR5k4HCGFx9cw19emOb157aTjscQAuqNBo4n2X9wkj+u38pnP3kR57z7OvzpLLbUuftPm+g/MMFffv0ROrrncdJHvsvskM2WnImh2bxnQZyBsRrDNUUsqpFxahzOQUc0yrTTQCtDPVdlKFtly3iV335tNUcvO447f3AbqbiJlAZSKXzhUWy4yLDOrLYYf7n/MZ5+eQDP9QiFdH583YW0t8TQjQjrn93Nb+5/DeX7OK7HPb/+Mm/unuDuP73E4jnNnH3KIkSonX/68C18bJFHrMVgcMF8vvPLx/jtjy5jyfL5PPTIJs696FdMTBUJRUwuu3A13/vGx9l2xcV879rvMi8l2DRYJmUa3PCjJ5BCcc1HT+EHN36Ysy/6KgcODdPRnqBRdnn01T30NId54qWD7Bn4LZue/CY/uPFumqZG2F60+dnXT2L2rDS+7xNv7eGLV/2AL1w6l5fuH2Y8M83zzxR47I1d/FN3E5Zdp3TkAO2zV9LZOMSWoSo3/uQJ+rrjrFvdR7xlES3zP4ChyQB8rOkkExEuetdyvnrVWdx+xw1ccdI56FIwMVXl6KOSxGIJavX6TKKgw2E3wr7hCv/6oRbmvO8ybrrkGo7rkkgn8PxXHLCFIG547N2wj89uO8z638RZs3I+Dzy2hXsfevOtckgBruvR0RLhox/5Z979C2jpSdIeUvz0R/fQf2gK1/WZN7uFb3/5HHRNEIun+Omvn+LZV/vxPI+mRIjf3v5tbjGSrNLL1H0I+6CEoGpXKJYCyZBrBwzLqayg0HA5ps2nYktmNQvanTr333w7NsFJ76knDnPF5Yv57fpD9HbGue+VAu997wVMTQxhNWyyuRzNza1kMxnsmgUKtuwbRHgWGc0kqQfUKlP5VH1F1NNZ1Kbxp31V4TreWxvCP6xTVPggNXqaYxRcQUh5NJka+FCpNZiqWqhKiQsuvoymeATDV/R2tdHV3s4pC9pRusHKRX28/srzdDe385mrPsHcVWdzywuCsZLFS3vzGALSEpZGJW0RH8cKbEgLk4KkgllNsKRdUCgGOsC2qCIZU0HJHxd0tigaNWiJQ8IUJEPBQKU5HUyFIzGwfMWhPOydVFRn6MDKVxTL9beK5JB7hG39D2OccQr7HUVE97EcSUKHZExyVLdkURIWJKB/6xG60zVMA1q7mkjoiky2SrHcQDMjbNoxwjHLFzCrZzb1Sg4hBaZpIKRGpXiED73/nYxPZjE1DVNKdF2j4Pos7ZmFY0iGChU+dtlaujuSyGQXNSdK35w2wpFQ8OX1JBOZOolwCJQfwHVVKEgaQhDSJZM5wcCRKbxQlKob2LCKJYu65VBXSQYHhshMOpTrFkelJOv3ldg57VJo+OQqBQ7kFLOSJrNaQ+hGhKIrmCrUGS7adM5uZen8JDdc92FkczP4gnLDBuXhuFD1fM48eTHrn9vHNTc8zIsv7OWZF/fz5EsDDI+XkFJiRlv50R2beOa5/by+ZZqNu4ucffG1XHb+sSjgyFgRhMARcbzRYZr0KDsrdX7+5FZuueFC5s+dxZPPHeLH//Eo71kYZWVbjKQU/Pyu17nkI9/j6JYp7rj3F/xhwwgrF/VSqdsYmqB/MIvUJNXxTdx398388K5X+O7NT7F33zRLZvXSN6+LrrYIxbKFH2tl38EcuzJVVq2ew+lr5+P7wSP0zMYxWvKDHFl/kGrvQkQ4ilbMce3xnZy7tplvXDmL2NBmOnqPwvYUfakwkxMFfnXvJjRN4pmtzI3CpUuaOX12jNlhn3KuyC/u3sCTL+0mZo1yQKUJCQ8zYpKMtdPVE6c5qaFJKOQc8r7DlDBZuWQ25525Arl0HtGYwhZgq+BluApDgZRg2R7likUo1sZv/ryfXS/tZf+GvRzcvJc9G/ay5/V+Hn1gO5nJw/hC46OXHM+Hv3Av9//mJba+vI9XXunnsef3Ua4GiZy2bOe7P3uBPa/tZfvGfh59dCefveYmZFc7vqcCLFlD4LsBVEKP6IQNaGuBsCmY2ylY2a2QhmDJHFjUoYgnBe1JycIWSTxm0NsW4o2XhtFbOvjYZ/+Nd6xdzeTIEXTVoDls8IEz1zE5OckZx61EMyWFfJZLz3037W0teDWLUt2mVnMQQtEd0hA+HCp4HDevVfF3FsS/K97uSseVKwyGHQMVTlJGI5JIc+bpp3PhuRdzzec/x4ffcz5WoUguN4XjehyzcC6XffAK/GoBx7GYN2cur2/bzXvPOpl/WjqHFcvXUu86DYAIkoSAiq0YrktKStEaVbiuYm57UMKOVsHQFZoBZUdQrwX2pphS+LaiJRGIt5UW9EksB8pl8PUZZb0IrICJaEBqievguR5/fmInpUqw83p2AbOyjQd/9SnWb7qHBR//GJtkmExN0Wp6tEroSUFXQrIi4vCT7z1IeypE0oWY6TOdrzCZqWAYMSYmsthOmGoph6ZJdA3amuNB9e07LOiS9CxbRaFUx7M93HCa3hXLKTsOmVyZtav6OOMdC/G8IIxH1wTRUAQhjf/udgjQfA9DBBCAhrKCa4BC0wJ7ogDqjgsiwEOJmW1RCImuSdIRjaSCvSMu4yWPQt2hrjymLZ1ZrRpNUZea30DUa/iNKtNVlyMVh8vOX0lrUxRrYisf+vhlTBermLpGyNRwlIfveiw7ups7/ut51rRG6ImHiRsarudz4EgmkOe4BuMjk3SFwszu6WN+eztDkxrFYgbPV+w7MInnQ6ni0SQ98prgoV1ZrnzfCSxd1EGxEeX6m37D1UdrvLfLRY+nmdXXRiKs8+Lrh3jyxW2k9SFu/ek32L13kHm9aXRNw9A1srkqmUyBsLWXa2+8nnBnmJUru1l69CyWLFhHa/ci1EwQUjhkIpo6OO3EBbQ0xwBFJNbMs89s54OnStLL2xkdz3Jw9xHOn63R/o4mUnMjxFt0CqU4LcYUTRWf5lCgZKjULBABGDmpe0RUnTAwJ27QkdSxLJuBw1kkVdKpJL6I4fgWliaoFxtEuyKYusd0qcZIts75Zy6jpzuNlRvg5h9ez94M6FqgNRRCBAJuGVDp3w5wMXTJcUk4IQ6rIpKFJiRjJktbBN/63n28/73HInQNuW+QE5ok8+OSZAiqDYcjowUEMJaziFWKrGmSLEkIVqYFj/95M8uOO4qDmSC4zQhLzJRGMiVoSfl0NQf073RTMKA1YpL2jgiCENU6zG8XeJqPbfsk8DgwbdF53od437lnEremOOec89i3/wA9HV1EIyEefvoZ+re+wQubtpKKJ5menqQwMcydt9zCP192ORee9x6OXXUsDcLkZISKFmVC6cQjCTSp/X9ztDQc58ztU1V0X2C5Ls2JGGEDRocGOXxwJ489/ijfueVHnPbOd/Fvn/oMui54/tU3uOqr19M3q5OQabB91w4O9u/j3vsexAyHWbt8KcIIKjRT+ISkYFYMTOnT1xzEhXakBBNFKHvQEobpusSQQTVoRhTZGuwdU2SqkgNZn2IdDKXQdALsejgo06u14EtiRgVmWNGZEBhCEZPwzMv7+fwND7FrYHImWNylURik2Rjh61efxsYtv+e8n1zPru7jeDkTopxTdCqfsBLMyeZp9hy6Yi6GbpDP18nkyohQjJHDw0RTKSLRTmwnqN7aWuLU6i6DI3ns8gg33/g50n3zmbV4EXN7m4naJbxijqrtcfkFq+loSwSLohCgfDyn8TZAW9Cf0zSNiG4SMQ2SRgghxFt+TTHz4Ek/yLQWb0s7E0DNhV15h6zrM+6BLxSGaVB3fGanQJM+I1mLQ8MlikWb8ZLFeNmhoz3Bmacs5cp/+z3TE0N844uX4jc1BW0K1yOmCZLJEPffu4Fj0ybtmqRTBulvvuvRPziNUIqSIxkbmyacSJGgTufsHtasWspTz+4g3RRjYHAa14Ny1aV7UQ+bDhbJ+ToXnrWMWCTG068dYVGtzP4pj/XTcPza2YQtj/bmOPW6y6PP7qOSHeKs0xYzVHKQjkvdduloS/Kzu9/kGz95klopw9olBq2L1rB9d5YI7STao0iv+NYJQmkamUKRY5f1IGTwOy2WZrR/AH+qhTf3FRkr5Gmb286vd5fpm/Io7rD53TMhvvTDS9j00m6ceIpM2cefEVfPSKjJWBG25YNcZsMAZYOu6Sw7qgPXA6tWoU02EHWPoek6mVKVNj2OlUhSrAXMxW/+6zl88it/YtfO3aw7vouDqQ5KNRWAkCXU64GUJmT87eBAAq0GxKKCEIrEmlUcdewKIkpROjjFdLHOk796gnf0Cea1QLvpBy2rms3QaB4h4MB4mcUR6ArD7BC0hWFNE+zc0Y+KCdoTEDV8DN9H2JAK+YRCIByfiAmd3QZh6TOdrdEo2ygfXtvlcfGyMImQIl+HNqCvp5e+jnaqtqKpKcl3vvcDBg7ux1ce3b1z6VuwgN5ZveDD16/+d2JtXXzh+m+ycfubvLFjMxOZSSJREzSNquMyXbI5VCz/X+rEv7Mo5ku1Z4YmJuc7bp2DoxM89+Zuntu8h/WvbmZifIQlHQmOn51iatdzPPLsc3zh41fhuA2OWrQEyw+xfWCI+/78KH++/25mtbWQL9c468RVbN64AU0IdCEIIyhZYCvJUCHwEdcais4kJM3gnzuqOUji06I+YV2QiCm6OgSuDxEjEGY3XMgVoNLwKeVVYCULgRSKTFaRywsKJWhH0BkS6JbNvQ9v4Yqr7+FHv3qBwaEcCIkmFJX8EG5xD+8/tYUH//xlHnzj97z3P77NxvhcChWFJqAtLpnwBGnPpVq3KZQa+HqYbKGBkz3C2Mg4vhM8CE2pOBOFMF//weOMjY2y7rg+6uUCPXYWVSxQHhvhUFkxb04bF529jKlsjRffOBi4a5SH53kIqf33fRTguIGv2LI9PKkh/sdtlABSRwW+iL+FASuQmkbVkRhITMfD98FAI1vxGZ/0GSvBaKZO3nHJ1iU52+WrV72Tp14Z5emX+3nqpX6wjvDNG6/BbniENUFUk/QBy6QdLJBCEQ+bNHyJ6yvGJosgJZVKoGn0HZejettpn7WAlq5eBCniySTZQg3H8anUXTqyWTb1Z+ntaWbh3FaMpg42bdpL++wWTlvXzlBkIfnxDCuWzcFXgrAhOHg4Q7lqkzCLHLduLfsOTWLqEqnplBth7n14K3f+eTPVXD/fuukK7nhhgHioQnX3RpTQ3npgLMsCoejpakL5ambzNKjni0TaBfGqYu9UGcO3cZpb+NIOiTh9Db3L+7jjnp3MmdtCuWEHC+LbxpsKjaZYg/fN9dg81eClIzaTSufmr53HPx0/H0uYuIUcYxUo2T4JXVCpudQNjVq+zOhYibNPX8ZQxuCZl/fzwBO70asH+eGtN3G4FGhdI1oQ51u1A/6o+u8Pp+5LdkwL9k1rPDQt+MZ1lzE0OE17BBakBPbrO1keUVQqkM1AQhNEAKthMzKexwhFGDwwSmsUElFojgUxwoYm6DpwgE4pyFcF1YqiUoJCxWPjQY3N/YqCJTk8oRgatcm5Jom2KHkUDWDNHMGj2yx0XxAxfGaFYepAP+mmOLO6utix5U3Wrl7FiuNPwxYhHGVw4rqTGRo+wne+8T1u+ekNhErDHLewlSbDZ9e+AV7a+CZvbN/Ppp37yeWLNGybwSPjwvu/xKLKvweQLZQbh7YcGBMJ4dIpFHPDgdd3475hfrn+Nf7w9IvMb09w7Wm9DGx+mBuuPB9Dk1SF4DOf+BwLUpILz30nfjXPgp4WhoeOMD05TVxCs5RoukJogpYI9KbANBRlH4oNiRkGX0DZDWIDrGpgTldCoIUCL3IqLPjrepEIQcSQdHWAHoFwciZSQBO0pBVdrYK4FMwPwTFJwbyoYGpoiu/89GnO/9jtfP37j/PC6weo1hw0TcNzalSmB6Cyj3PWxNjy2s+4+HtfY8pVVM0wCalhKR+lFNl8Fc8TJFWeRt3HcWxsx8XQJKGwQcWOsv7ZPTz05C688kHOvvh8Nh2extc9xqs6k5kiH37vcfTNm8fXfvAUb+4YASkCJ4XvBOynmXviuh6aLnE18JSP5wTDCvG/+sJ/hQD/7a4YktATN5jfrDErJok3RQNsve9RrPpMVWyKNRdhRqm7Grlag0gizEc+dB6/vutplOvzwGM7mB45wKUXn0hBE3TqHj2Gz9yoJKEbxHVJBZ0tFYEzU+fmCnVcpTE6nCWCTlQDY8FxJGIaqXQL0WQcXUmqdQfbhdHRDI4RZrjsMH92K+GQDokmBveNcdmJHTy/00daWRxLsrU/w9GzU3g+DI/nqdYsvHqBd51xPL4m36qWhRBYtsd/3vkau/onqR95nd/ffj0XfPEXOBLscvWtssr1PCJhg5Cpz1xBBWiokEY5V+a44+L867Ioz+6ZJD+cxZos8PKLk9z34CbyhSKtqQhTFUGm5gYDlZn74dWm+MK3b6Dlso/zwL3X8frG7zO44Rt8/PK1tHY2869fuY2EcsnXIR6KM3BgFM0T5HJlEi0hcpbLNZ98Nx/+zI+JSFj//F6GR8Y454xFqEVLUZ5A6D6RuAiusTbz2ULQqOV44Hf/ysPDt/NA/y84sPVGjl58FE6jQtqUdDcJlvVqdHdDUwqOmq9oSQcYPaWgVGmgm1HGRyew6pApB4RvbJ84gqgJ6Wgg9J7VDZ0L4kRiBl0Jj2Q0uAjhsKTmhvAtGDncoFYNbLa7xgO4RHMqKGCmlOBPd93F8lPfRXbPBpp1QW+7yXevv5ZwOEHJhUW9HXzpYxfzxr038pNPXMzA8Ci/+OOz3PPURiZzFZQP85KCbl1heFVGxiaF73n/f4AQClB4vo/hNJjdLFjcIeiLK97ZJRGeYv9Ins/f8Sxfv/Nxrlk7m/rADi4/YR5nLTuaD1xyLqXpgwweGMAqTLPqxJO49NNfREoxs3H6+ApCAiKmz2TZx/ZmbHGuR7E8QwV2FQ07yHB2VYBJt+oQneEexgzoSAZEs5ipMITCrQsqJbCkBNenUQXfUsRC0GbAnDCsTMLRccHckCI7kuUXv3mJD119D1dc83t+ec8GBodzKAWaFLhujdrEVj7/sZNpO/c8/LCO8nxcZSCAfLGG0/CpuU1MjY9Rb1h4dgPT0IiGTVyl4XmK+9dvJ5cZ5fOfvoiiJ+g/UGZ0IstRCzq44MylZAtw30MbqdaCHFvXF1huA5QXTNI9m6VLFqADYQIVviaD6/j2Q5IK3oz4P/xeF9AELAxZpEIgXIXQNKoO5JwgrMiQEt338HxF0fa45LyV/OZPW/Asi2RTnMGRHJWaRVJM8LWbvgwNh4Su4WMy7Wq8XjN5o66hAYbSAMl0tozjSEYnCuiapGfxQuoHdiOkRPOrRJvTlKv1QLqBxuhYhvHpAraviEbMwDggdLpaBctaG2zICaxyg0K9X0bZ4wAAIABJREFUzpqlHTRqNqYUNCwP11N4jsWSxbNpNHwMwwAFnlLMiWkMjxX49q3PkC8USWpH+MHPvss9j7xCSLhvM3mJGQ3m3/bkZh29ks51H+eVHTnCkRCrukyyUjAwlWNg316UUkwXSxQms/TMSuDN6F7/+n6rPMh7Tm3mgtPnsnC2weTgYZ59eR+/uPs1Tjr/Rzz/yGuYpo5lu5x6TJpXNzaIJU0mi3WmC3VOWzufih3GrVkk00nKVYvpbAV7ehcf/dLH2V8JLCZeIwCnuG+rFA1NcPttd3P++d/gvPNv4pRLf8beff1ITSdhQk9UIS0fuy4wgEoj6OkbMlgoRiZKKBlmcjRDLAS+7yMQhMOCmOYj3AAeUfcUtgNhUcfAxQwDWnAh4wlBOgkyLEm1x4i1Jkh3p1kwJ8mKxVFSSYNTl+qcschgeQy+9u9f4ZgTTsI0ffbu2EbEHuRb136dDxy/mKUtPvPLA5zY18r7vncP2/sP0N0kOTqtc8NpYc7sC4Toa3uhXLP++I9yFv8hSs54Q4qaBZcuC9HblUJ2pzFCAl0okoZk/Y4RWq7+HcOZHOb0GN7uJzm4bQeTU4r0oV28o6uJ4UN7KDseQkFCQncIlrQGxI5SQzC7RZIKCZI6dKYlvW2QMn2iOnSlIN6mSLeBZvg0JRTKEJRqAfIrbyk0Q6CkYroosX1FozwjatUF8XiwEBfqCk0o2sLQl4Cjk9CNYklUsCAi0Ct1Xn5pP1/81l9476fu5Cs3P8qbO0dQCoTUqE3t4vb/+DIDJRjP1ql6HiEJtbpNreETEj6yViMznadSt4nHTGLREPV6A4Cde8d55KldtHTBqrXryFdKVFyPKy5axYL5c/ngp2/F/+tRy3PRpY4RMvHVTKWoXExdIx4OY6KI6AJdN9F1iXpbRSgJWgu6CEK1/marE5K66zBWkUyVLMquRdl2cTyPsCFJhA0s6WNoAsd1MCImn/7gOtat6uSh3/8bf/ntVVx2/kqe33CIWnGMqz7xbvqjTRxxDDbVPPbWNSqeJK4kCImSQcuhUGxge1AoVomEQhj5EmYsRte8btq6O0hGwkjlUa3aeFJjbCxDPRFDSN6a/IJC93y+86yFqmbIVxT5fBXp2uSLNol4CE2XSBm8pqemCYf1t66MLyQrWkP0tsZ55uV+7vrTm3iNHMcv0jjjsvdzZGzy7QEb+L7Cc9Xf1AnVqRHWfOKLvJCJc/9AgYG8GxBgmuIcmC5hKodjV68gS5hVXe0ULXsmTnWGwKLpbHzzAKVyhe0H4Nwrf8XFH7ud7/zwUZzpPN2tcequz8lL23hmR47WhItULvVKgwPDBT59xcm0xDweuPdL/PmOf+ELHzuF9c/vpV7N8YELV1OatZB4QhCOBbrbt2YKSqGF0jz16gS5XUNM7B2icHCCO//wPLO604wWwXIlqCByoGoLPBdKJYFyg+VkaroEukl+PENrDJoSAaS5VgPbCU4FjZnNZ7oI1bJHwxZMjYPuS1yhUy0JqhWPas3Gseo06g3qlRK2apAve7g2lIsehbxNa1Sy96UNnHTR5Sxyc8yRitce2EB94gD6gdc4StX5xXN7OPm7fyFbbGCYkMnXyNddHjikKEdTnLLQxEVQaHCp/w9yxP6hRTEigiCOE1fN5tPLJSv9LE9evYgT5zUzrzPC+xZIlIIbHtvFNb9/ntNXLmD7n/+TWUuXsq9/iBMvvYzvf/MHRHRBTCjSIvjkybIiFQ5iSS1bkYz4VGcYbdUylBuSeDRwPxQmBJW8wkZQbwhSEUXYhFoDvIagXlVvyVGTYUEs7GP5gkoV6lUwDEVXTDF11FEUhCKkK+KaYHFKMMeAtU2KlUnB0oRgtqnIjkxx2+9e4wOfu5s//GUrKIXvOiTCJcz5R1NoeOgiGOZAQOyJU8crjjE2lcGybQxdJx4Pk8lViIVMGpbLHfdvIrPnDT591aWMlOvMm93CJeesJFPReemVnRgzTX3sOoYucSwf/20NRSEUDcemNxocjTzPRicQ1Tuuj+8rdEDXNMK6+F832Fc+eU8wWPcpupJi3aXs+sRNnSg+Wd/mhDPW4Do2Jcfj0gtXk6uGueis6/jUpd/h3Iu+xw0/fpLv3/YcE1NFGpPbufKqK9kxmQ+GQAJiQhGTPjGhCEmJFIJcsYbnSbLFGvNbWynXG8w9+SRq0xncag0Nh3A0gRAgNINKzSI1pwfhQDZfwfOCazCQ99g83MBzPDL5ColElH3DE2iGR7Xh0paOEQ3rCMNk284BivkaIT3or7quz/L2MLOjGhFd8LO7XuGFDYewyoNc+6WLOPH0d6Lr8q1rXWvY5Es15F93Fh3McJrq1md4dtvzpHuX0zlrFvlyFTMUpuErWpvbOHJ4hKgRwisUSScSb1tUfaJNczj18v/ktt+9wrrj+vjpLTeiAws7E5hasPLGOjUKXp09I6AJgaugWHeY19vGgqOOZsVp1/HBC27iggu/yxeuf4D/uud1+gezWNO7+OGt17N3RBHxAqufpr2tPhKgaYLTumB5i2RJMzz78MuUHRsTRaXqYTuKmh04yqpVhVP3MWUgi8sV6yh0PDdoVVlWkMGeTIMeg5A5Q+B2JULXmSjpuLqBntQhIgmbLrZpEI3rJCMQCflENQ9delg1B+k7hKVNOuETNySGp0jbNe795Z2cfMIyXt0/yEknrmD//T+lvTXFaT96hK0jOT55cheLW30+OMfDTKb52OkdvHe+YpmWp6s5KBek/MdJifLvB0IHPytCisUdkt61cxFrTuKm332FaCyKqbnkCi6HSoqVc5pJRQ0O56qcc+sz1FWFF3/4Nc77zPtwSkOs33EIQ8EsKeiJgS4lLYagXBe0pwSaIRgqCDQVQC49DcyQwvaCIQoSPA9UJdDeVWxBS1PAlEu0asQToOsCw4BiPYBBeErha4CpsBBkLIEWMhhLzkU4AnyQoQAlpvuSqAer4nB8WrE0LemNCiYm8tzw4yfZvHMEXZfUskdYtHQRhVqDlCExpaBStynXHHoTOlJJQrU6ruOSiIeIxUL4vktzupPI/8Pae4brWZVp/7+17n4/dfeSXklCQhIgBIEAIghRARlEHNSxAIo6OpbRGbGi6DBWBAV1UBQVBUcpFpp0CJ2QhFRC6m7Z/el3Xev/4d469X1njuP/7o/Jc+zsPPu5r3Wt6zrP3+l6bN8zzENP7uTUY3vp7e3i0redwFErlnLll39FwQT17xsTrWhWG+g0K3ZxotAanNlzGcBkhRPTayiMGRW+EMzQQf7NUvdfOkUlqUWK6VgzGadEpk3eNdAoRsKAKz/7Vob2jVHwbaqx4kOXvp5PffFX+O1lRqwCynZxjOwKeu9juyGp8Km/fTPlWb1IaeJLaJMqu4oKSSxMNBmgVGlNmsQYUQszqFE7sJOOWbMo5wTlQh7pGCSJIkEyNTGJY5dQGvYeGGdyugH1CitWLKPabNGKDJJY0YoDqtMtDC2J4pRF8zoo5B0ELn+470ne+e5LKLa3ZYcamllFi2O7HRbM72R8osG3bnqYyekWzeEX+e4/XcYFZx/9F3G/TjU7dh/Jru4AUR3TFRweOsTgC5v4yW9+zWeu+RbCdKk36nTnPJ7ce5iVi/qZ1VPksOmxvLuN+N83KE47YZhy3c1P8Itf3sXbzlvEtd/5AkemmpgGCAeOmWPz8A5Y1qYItEEjipiotvjy32/kE1+8naUFj86yTa7s4VuSsfEad92/nTSscOLKMkfmLMWws1Q9rf7rVCxogB9qjFCyNGkw+8A+TCnQWpDG0KhmWsN8h8ArZSwCKSCMEkSqqVWqREkGjjAtRRRoDK3I+2BYGXvRRNHpRLgqIqdibJUV3kKS2UWFFlSbNjnfpN036M4r8l7GKdUzAVkLZgnm9Uh++P0fwIoFbDx2KXf8y9UMTje46EdPkipNf5vg6X0tOh3Jp59ICRsRZ5zcy7lnn8D3vvsOaqrAWUebKKX/Ml8X4j9E7v3viqJA4JmS7rycXNkh9adOzunpSLH2w8/xyW/ez1lvuoGfPfgKsWmwelbCuuU5FvfYLOuUnLbIorcguPL25xkbO8CRBx7hKzfeBkCvENiG4FBdYMaK8Rnr4PAk1GJFyYVIaeIoSwZz7Wxd2tENjg/SExhFjTI1bbbG86GrJFCNFCEyqo5lQgJoqYkVOCLLohipwngATz6+g9f+1Rk8d0jj2BojzfSAhZxmTgF8G2aVBMfkFIs86LIEw6NVHntmH4Yh0UmAbYJlWizyE5qJoNmMMKSgpS3CWOFakiRNyLkWbQWfnbv2M3dWPwXPo9FI+NHtz1Ib2cWmuz/De9+6nj37Gvz4lw+AYf2Ha7BOUoYPHUCiqdVDGo0QIQRx0KDlt/FMUqTNhnYjzTo0zyaOEzygzZLYUs7MFf/ty5aKTiMbS7gSHCNz/oSpJvYcTj5pPfu27Ga0FnLqhsW0AoMtO/dRTyQFw6ZouximSbMVcf+je4iTBNXaz0f//jImq80MzYZiCpsQiaVTTKmzn11aHD44gqEFc5fPYc66k5DBJBpJmkZYQDOMSZIEKSQL+9so522GjlS495HdBJNDbNx4MjunW6g0IU4TFIpGNWKyEZJqOP3ExdiWxaGJmMmdr+JbYzSnxzONYKrp7HNYt66f9Us7mdNd4tGn93Htjx4nTUJyeh/f+PxbMFwnm8GheGbLwRl5FUTNKWYvXMSTd93JK5ue4tU7v8kCa4xdA0PkOnuRwiBMIvaP1Ng1WiPBpBYl/wVvbwCTEzW+8O0HeOD3f+CKd5/ABe/4aw6N1+nuzNOsg206vOWjn6N79iwajSa9PXnOPus0dry4F5nPUzd9HCEwDAtSze/+tJ3RiQZp6yA33PBlXprI5omJ+s8CFE0tBFNq2g2NKyU5RxAmGomm2YJym8DLQRBAmGiCP0NqoxQhLbRKsYws8rRSlTMRJQItNJbUSKDgptgFiZOXuGWHJDWIlSbWglzJplFXREHAyHhMs5EyMiUYm4apiqARQRxqugsmnbZgenCA4eFJhu+4lQOD43zriV28/TV9nHFUNxef4POeU/MciQy+d2GB1y5Muf7WvXzn9m0c+7bf8svnGty6RfCafqFXdgu9tN3QPTlLa/F/7RT/69/OcbQ+ul3p5f1G26KS5qHdIUvm5FhjTPDBDSXm5COe3dvgwJ4phFNiUX+RsDHGX53gE2nBG1YX6MgZfPTuLVz4zZ/znV/cQ97ITpwOR7OyDVJDEwXQjASmo5GpxDazKADDELQSSbMFJIIjIxBWwQ40jUpW6icTOLBfM1WDSEkaoWC6BfsnoNnKrth5R5N3M2J20dW0yMjCJ6+dhX/GmUw3NLUgu65HM7QdNZOKpiOYU4Cyk32kersKaKWx/QJDh4dwhWA49TBmQryiKCZpRdRrIfVmgEpSfN/CNG1SJE44TDHn4/kem54/yH0Pv0hfOaCrq49rf3gvy0oWYRID4t9maCIbRsVxQhin1OtNyqUCMZkEJrVMHm7ZDKYmqdKcdNw89hwcYb4NHSLEEel/mGcBGNKgzbbISwPLMLAND4OUEZXy25veyy9//ThtjmBPpLj+6r/hK9f+nsXtnfgywTNUJmIGLCF45qWDbNs9TFAZ4NK3biA0FFKktLTMDtaZZERLiAxpD1i2SaJjlpz7TqiOIqw8hmVRKJcyH60QSNOkHjQp7d6GaSrMOOJffvk0hw4PccZrFtDmFZmp5cQRtCLFWCVgwbx2Nr52GV65g+tvvJtjeoo0Bw/PnPwCjcbq9OnsaOP4uZKjFxQpO3DLb57jT0/sRYqEMAgykbOGnKl57Jl97Ds0kW2uW1VOOmkF28YlG859PX1zO3jqF9cxfuBP/PGen7P34DBtfo6x6QplERM2Q8Iko9T8u/YfV4DvGBw4OMGnv3Yv2555mGu/+nb+6l3vRTcqpLlV3HbP/Zz/lot4+IUtjIchn/7wRj7xpV+TMy1MIbCNHIbh45sS05Ts3T/O05sPQdJg3ZoOwvlrKJga9z/pFLUAX4KLQSGXaXy1hlJOgxL4XrZfatUyaocWkijJuqcwiokSRVivg9YkqSZX1Ni2xnYEjgHCAluDsA1ELJBS4lhQLJg4eZsIxXQlRbhgWRLPzBRL1Vb2b0idjdh8Dw4Np3gSXtsBn73wI7ztBw/y06f2U8gJfCfFcOC2pxrc+MQ0r05oHjjssGXK4uFdTR46lOAbCW1uxKsDIaZdRk0qclFKvxEzS/93dTGbwZt9efTzH+vD9fMMdPfR7xahMsm2A3WW9OUYnaritrWhAs3KtYugo4+BbbuYfeLJ1JsVqnv2sf3gIMNiHicc30YrN8K2QwlHJrbiFT02D0xiScEcCW0zmqfDtSyxrduDnKEpuCAtQcFX6A5JbVJR9CH1wfQk9rQijMF0NWkTkmYmrykv1dRrkrCZ+bIdIG9nwfWJhDAArQRdBUWzLpGmotuHb97wG+66/TrOPq+JfnoTx/RlCPVWoknr0N4HqRRMT2n2VuHjl5/GxjOWkyhA5HjxsWfo8RxipTAFRElKM0yp1AKkIWjGikjDicfOR0iB4/okYUKbazFpaMZrEbf97iXOPm0ZDWXzx7sfp8PPYwQ1lNb4rpXJcAwTmSqkEBwZq/LcS/u45D2nIw2TVw4OUC7mIYoZrrc4ce1c3veuM/nQ5//Aq3gMhh5RGhAosEwDyzIROmXZmhWEUY2iznpSrWHhnAI/uuQE1q1bz7s++ve82tJ84J0nkivP5fFHXmJBycexoKEEQkjyhiQ2JWMTde59ZDerl/fj6GFu+P5X+OwHPku5UKCoEnxHcrhuIIRBksYEkeLgvkHSIMTQTaRTRDXG0FGKDEMs2yBVCiVMGpFi9fJZODsrGEHA5u2DXPm1e/nWF0o8/uTPWb3qPDqNFJRiYKKB11Xg2i+8maWL+vjp7Zu462cP8u33FPnNMxFRFNFe8th/JEV2eVhlyVFtazhh/BVGBkZ44UCda254kGWLu5nbX85uubam3bfYMzjJr+7ezOf+7vXoJOaUExbygR0DXLTxUoxmhXdc8UZe+sYnWfa2y/jnKz/A1T/6DWE0jl9eiBYtUp3Rq6WUf+lDUg29nk291eKFrYf55Fd+z/eutvn6VRdz+Ycn+MxXrqezo51X9r1KnCo2nr6Uc994Gpd//HLOWNALWjEaJEhcPCfFjmKqQcQfHtzBheesojG4hU9f9WHe/4ZL0W5Gscn5FmhBK1AYEkJSvCiLQQ1a2VJFoUm1wFEaz4PpOgSSf5tL6gwMEccJo4FAJ1nAlWNnBKKK1FhuNv4yhxVWnyYSipqAak0wNa0wLPCNlLwvqDSzdyRnQqefUZaaGqZSQSUwMKIZ+YwHVnOKlcJkCshNJvy2WmFBXzs/eN8SDg81ueFPE/z0XYvI+0WShb3Y00NZZe8poPt6UU9tRh5/Mmpggn0HKszptjH6j9ZDSQNn+35SG6790S6+8WQozOE6YtaXh7NuiFe0OaMPHG1l11B7RsqhgHn5JzFmYiFX9N/Fq0PZDOShmy5g8oFnsHSRnUNNLn3rOpYt7OeWP21luhYg0UwpsLWgZAn6DOi0s+FnIqASCkppSi0R1KcVpSLIRGezjEBjWxAqSC2J6yqyhkrTmsgcGUIY2DrFcbNfTqgyplGcSHwnIxn7rsaIoNGAPfsnGd7zJPf9/kvc8KM/cedNt9M8OIgVh7SX4JUJk8jyKC2czZ1Xn8pxq2bhOSaOX+Q9H/8BfaQobTHdyCx2cZwghMA0JFORZiKJOef0ZbzlDaupVKa49K9P54Hf38/EwCi21NhS8tBTr7DnwBTf/sn95A1JhIGWBuvX9HP2a5fTmBzkY1eczxcmJ3hpyx6iZsQ1NzxEueiz45lrufX2R3lh2x5IQk5c289Jx87lsU17uPvuTfTkc4RpRKMVsWpFH1/+xEZ83yFpHOSeX34QdPzvxOCKuFEhjCLqkctEpcJVHzubS992ArPmdvO+D13Mb26+Ay+V1HWKo1O0aSATA6VSgiAmSTRJZYhL3nw8//KT1TRe2Mr8kkNLKVKdYgg1g6aTaKW44Aufw7Vjpg/sI7RcyjoktnOYgEoVoEhqDcbbFvDm/p38NDYpiITf3buNoSMVrvr4ADtfvJlvfe83DAwMcv68Ehe9cRW2ZfCNHzzI5675A7edOpvxZR477z7EsqPnc9Ypi3jP5Wdw+eWf5sxjJpifNOgb3MKyksdhP2LztgGuvekxrvn0G8iZkHdMlrZb7JuIuPWuzWw8fRnHrppFPLqbf/6nT/Djq69mKGmwdXCAZx8+yBsqt3LRBy7kgUd7eW73ISYnx5FOgfHpadatncsH3v4aogS8aJyr/+njfOdbN7PhpGM4NDLG/Y/tYdPz++nveZEffvcDfPRTHyM1OhkZOcIV7ziRT1x+Kh2lHP/81b/jp9+9lS47YU4hx/7pCBMX26xDRNbFxSlJGvG6DUfz+ssu5q47/8jn3n0Kixd0E7YmuPrLV/DRd+xjaWuCepbsQbmQRQEnSmRmiBnRt+NBztOE49mMOghTkiRBxCHlnEB7gjTUhLEgNTRCSVRLYWiJU4bYEqTKxuu0mG4FtLULjFShhESh6O81EEFKEEPOFYQSem0YnADf00xLTSuGcxcJ/vgy9M0RdI/BjlXz+cYSgzt3wovDCUcvW4797BbOu34X8VidSgwLCyb7BkMWtcFLU1ktq7ODBlCcQdqtKr7EUA0G/+JwEP8ZHZZtbg1DsKxN6g+uFYzGJndtC6jFgvmlP4MXFBuO7uDgRIudRyJ2jiS4xTx5U3NgpEWhUOCcDXNwmnVOP28db7jiN/ha4wnoMaHPyPRSRVtTT6AkobMAYZqhiQpFTdAS6BhMQ5H3BI0G+CWNrWGiKjD/XFBTUEIgpEnQivHsbI7SigWVQOOQOV6KeU2qJK+GihemQOV9rvvSBZx/1gpMu4jXu4ihfaPsGxwhDlvM7vFZOKeAUAH1WoBhSFzX4af/+gK/+NzP6et1GFMuQ/WUI9UG55x/HO9+1zv51NuvZEhoTj55STZjjBK01vR3e5xw/Bq27jjEz295iFRANUj49lUX8rWv3cOCzhyvVpqcfNJiXMegUg8BmNdXYM3qFfzxoW384Z7nSRSYtmT54m7WrOjl6KP6qTcjtu0c4bkthxk6UmNuuQBpynTQYv1JS1i5pIdmEM8wM/X/0eDUXvY5ODBFZ3uOOE4zoa0lOPaYJXT29PCe93+fsoowTZMwhtEk5MQTFzCrq4TtGmgtKOZNznrteu59bCebfvoAi0uCA6HBwUbEeKK46h/eyD0PHuKNb/krgkoNvCJt9WGaqcmrB1/hyS172Lp9L7uf+jqX/d0tbEhGOW1pB9c8tY8DdYWVJkyEkulQsWJxJ6/bsITFC7rZvmeUbbuGeHn7MAWdsnB2jr+ZW+DGiYC1yxfTXvaoVAM6211OO/l4Nm3ez77N23n38Z289Owuvv3sBEkKTcPm/LNWsvG1K7j7jmeZPTXELTvrjDZTzjrtKH789bdRLDg47WtYeOx7Ob4zoL9T8sJ+n3ceW0a7Fmdf8iZ+fPdL3PCzh/BdgzNPW8Ls3hLVWojS2QG/ZuVCOrq6+dqNjzA0MEJXh8nyxT14rolrC45esZDu9jIPb3qJMFAIKTANWL6kn5Url/Hxf7yF/jRhXzUijHzGm6Mcs6afBbM78D2LVGt8V7Ju7QoODQxzcHDyz8tv5s0use74tVxz3d3M2v8yfgS2m12Tg0BlGlgvKwfVRKIMxcuTcKimiYo5rv/q27jpCz/jGDejZwmhkIbE8wVJrGk1wTU05RIUioKgqehrg9HJLCZEpdmzaxgg4mwxKtE0miblEuwZSJhVBJUI9k0qLFOSVxCnmpcn4LPvnY3q7WTbuGLevF5+fPuzbN47xTnr5/DQiwOsnG1y/LIyRRRbD03RqTX3HcgMIHGkOXG2ycXH+/zy2Qp/OCRFJVD/DqL2f+UpZn+8oM3Qv35nNz97ZowjlYRHDmtcL8+SvjyD+49Q15r3n7uEl4bGeP6laTodQWgJtg4pNp5/MW88/wpuv+VqnnzkQXqEoCihXUB/XtOdE9RDGI+g24OCCzkrix7N+YJYZaJRIWGqnqHZEdBski06dIYxcyxoRCaTFUWSKko56CiA5QhaoebAMMyba5C2UgYDwbMTmsEQunqK/NU5K7lw42oWz+uks6OAYcrMXqwUqVIEYUwYJmzZPcx3fvQYyQs7WZSXhKbFkJlnuBqxZ7zOxrNXcuZr13P1Z36MMiT1OEEKAyFAkmVFeFLjWTa2I6mGCYHK6Mxz2/KErYCW0lSbEa0wc9QoLZA6xRUC0zSIUkUiIFEKJQTpzCbaAGxDk/NcXNtGaE2sNI04IoxT4jSL2VKQfU8hs6wUIZEzAn1jJuTdEhqtsutVpCSJMLCEpGBKOlxJ0RKYWtNQmvFQU09jokQTa4knEvosQZcFJc8nNgy01qSpphaFSNOgFLaYf9a5LFmzilCa5OMGVWFhW5LhA7t4+MXdvPDCDu68+f1869oHuPioPg5MD+M6RZ595VVGKmmWNxMJKs1mtqnXkLMtPEtScAwQmvacw4mzbSZyJrsPVAkSiW2AFyasWlli6cJuFneUSWtNXnpsF3cOhOQsQVNJervKrFpS5OzX9PLwb3Zz68sT7KslJMB733oCV/39ORQLZaL8sRy1dCMb1+e47+kmH1llUOwrYeXbmLd6HeNhxGMHXIb372Dz5m2UeuewcHY/f7j3Po47bj0f/8SH+PCl7+P8Sy5maHiCMIxwXJvenl6aYYxrWxw5MkLYCnj80UfI2x5FM6Wcz1G2QSOxbZe9E00ipWgG04SxQguLnk4bc6JOGEMkoa6yz0mfA91OptH1LYmZYRUp+wqlBFPNDKpStCAxBbFjYJopT+/POKOpgqLW9PvZiMo1s02xZUNewmQjkz2zix2JAAAgAElEQVR4jsb2BSVPECQSC4VtgTA0R4ZnXu9rHDRaSKqxJG8JgkamsIgSTS6FZ8eh1wHHFoyNKlZdfzPFyOaHt36d7S+8xCkLHPJGzHRdcWBa8om/28Ajv32SnVNwzlEFBqcVkhZTtZh2U/MP5/Zy3dMR//rClAiTLNeJ/0a7+D9KvB1b0udpXXDgjQsdzjtzMQNDR5i7ehGXfGU7Z60ps3nHGIkhqbnzmLtgKf/wj5/h4IEJhgf3seGsc3nTaUuZq0L8DKBBogV9vsIWgi5P04wkpgDD1Dhulr9SMmG6BokQtLdrGgnENU1jRorjl2bS/FKo1QRTlWx7ZlkC38jM8JYDUQt8VyAdqCWa7SNwOILxUFNLoa0jz5IFXaw6qo/5c9oxTcn4VIPJSpOBoWkO7x8lHJvgaAssJFLD4g7FoNPO5tGYvlkOOwcaTDYiZJCigLJrIiwruw4KMIREaoVj2cRxhNKZdMaWWchQMsOr1EjiOMqGvUAiLaQCyLaxGoGWWYavIJtTmWikNJFmJnsxycSzYZqilSZRGiUyUS4iK4ypSjFmNjASjS2yw8cVgpyMqSUWU1oi0NlQXhq0G5q8kRKr7OcM05Q6Bs1UkpcJ3bbA1hpHCmzbJKdjbGlSTzUR4FkOh9wyp7/jbdi2RaItVJpgmiZSJVTHR3jm+ecZr7cYGxjl+9d8kDtu/i0qqqB9l8bUNIfGG8SWD/VmphM1s4VOzjWybJogRNou554+l2CswvIVNtWKi+FIUlpUXhpl0fo5lAyBF2ryQcADu0Z5YjAg55qsm9dGpSaRUUiiIgYnmhxIBNunY6ZbCaZl8O6LTuDKD51BX98cthzM8Za3fghPC/oKsKHfpKenE2VZ2O0dmMLnuSHNkhNPIZ0e5R+++h2u/Mxn+PBH/pYzX3MCb3/nebzpoisYnxhHK41j2QyPHMbNd5NEmfd68cL5bDzlFPoKPkEk8WyDkqsIgwglYKIliJVDqipYRpYdtGBhkXlWQmXPILVE4soslzxEUbayEKq8Dbl81lyESZadYujseUpFtpQSrkmYpISJZmxa0JnLCPeOkS1wYikwhMJ1BPHM4ssxoZDLTmHT1GjLoKNTs2+rxsllJopWqHCUoJAXjFeyOpDo7PWdeclYXSMsODyYxZJMBgK1/gw++vnvsGPr8yxfvoZdh17mV7+9iz2P/Y5SDqYqMSef2MumzeMs7xa8ebXPK0egFQZsHYppRSmV1ODAWDLzv/sfI07/z19pKvjkcXzx5GPa2Tph0QpS1q9fzZ6tuzAKHdTqAVsmC5Dv5n2Xf4r7HryDS95yCVu37SSMI+Z15xidmmZi+3ZsKejLSXyhsRHkPGi3BVGiWdSfbfySmTS+aiNrsQuupt4URKEmVgLfyYSiQgkQAplAGGW/XFMKkliSoImTLLY0aEnyriaNNFEosFIo25k9sMsTxPWIyug0L249zKNP7mHTE7vZ9sJ+JnYOYEyMs8RqMV/KDMeVZtCBRIMZtcjP6uLEVT1ceFKJJyuz+e7Vl7Py+OMYGRsnqtWIgkxYjZjpyeIYpECqFC01KtFIx0bFcdahzTARUQKFyYltki5TMZEKXMOYiUowsC2BIySObWJYNpKMraiT7C4kZKafdI1s++f7HpYFKIVQmnKhjGlYSAEF28QxJCYGjgGxdGkoA8MwyQvwTUlOKrrtlFasSaXEMQU5L0esFJbp0e3oGWSVpGAKTCmQhp3JdQxJBYediUOBOnNWrsEwLBwdY7sOdm2cuHkEYfk4+SJdvuSo4xbTqAqOOe4otjz1JNVaitdWJm00sQolWlGAoTSGNDBM8MzMxpcgEIamv38Wy+f10qlqdLiKvr4CXhSiD7Rom+2TUymdOYeRoSrTdcUfBiPW9fpIbTLeanI4NjjSCKlgUUtTbNOilWiSVLH55UF27RtjxaISa1d0csFFb+drN95OzjCRcUgwVWd4YpojYw12bx7ggk99Bg7t5ENXX8cdt/+UVUcvYdeufTz/8MN84ctfwXIKdHZ0Mau/l1n93Tzw2GY2nLyeQrHIrNmzKbfnGZtqcnjz03QUTCqhjUBh+Q5plBBqBzAyS50w8GxJvZFj/jE+7bFG1JtYAnK2oIhAzBDqpxoSQ2viWFAJBHGqmapnhU2hiRKBIUDbBrqpKDgCQ4HjaqSAZpJFHkgBQSSz2AIFpZzAEmB7GteU1KY0dgqGM3O7SwQlRyBk9rzaIsutLnpg25JIWLQXJUXbwBSayTpMVDXvu/kWXt22h0RKXtq5k/XrVvOTH93IW97+EfYNDRIrk785I8dwRfGrr57OnlfHWDnX4Janqvzufe088EKDDlezr6Kv+p9qnvk/67sVL0/aDA5MccZSKHb28pPnatz06wbnXnAaXQv6OLl3mMVLjud3d9+EqRXdfW04vsus3g5u//0T/M1lH+KiO25nXiwYn1Z0SihYmj4Fg1WJZwmmK9ldUBgZHLOjIGgEmnogMRxNzpIEcTb81UGmVRQGmK7AChVVJEFLYwpFwQNTQ6EskAqmqmBJQTMCLaFbKno9gVYwbWZpgdrI0GVojakEHXmBnPFw9rRrpmOYmAZDpEw3ssTAjed009Vd4rEnXuZXd7/InqcfxJ8XcMmHP0hXRycv79jFE3c/yoGtzxC3QlxLIkw7I9gkkBoJuhVm/LpU4ZkmnmMRJymhSmm3Qsr5lNHUp4LAjFISoZCpQEuJ1gIbQb1RJyh3s2jtqex77mFKbWVy5Tx+Rw8iamFYKVONEBGmeDkPW9Vx812M7z9AoDxoNcnlSzSDKvVWgpQBOZHNIG0B3ZamlUoahqDDsDEE1OI0K0oixdcCYRiYGjzqSHziNEWj2N5yEEJiEbL8jA3kSh3UYwGFAk6rRiXXjuG2EwxvJWcalPq7qIcpk+ERlratoXvRUvbsfIX68CiOl6NaqVLuamfiwCiJSPFdnzAOcV0TEWtaYcJIFDM7sZidN4lUwsiWIXKeRWpJWhNV8vM62L9/H9XEZag+xRxXcrBl4oURR6om02GDJNUIlWLakvacpJU6TLcigjjlnod2sPfAGB98537effGpDB26j+OOv4RFF76TT175ab73netZcdRCjjv5NBrDr7Lhguv4yAevoCtfIrQsrv3U+znjzFOoxYJjVixg+5ZXue/uR3nj+a9jdDLipW27WLZ0Ec16naAp+dgn3s+t13+LVTkfnYI0DQ6PJbimQ6wgjpoY+XZEUsEwcph+maRRYbfRS90xWJ5MIUjAE5hpFjTmWRqtJVpC3oKpIBvDKCkwtaTkaWxfMplIgliT8zJnWRRk0qqcqxE2WIZEaI3pCmyX7OBUAhFJmqkmTWBxG4gJCGNN2deYJhAJJupZNkycahp1Qaw1uXIGWRk4lFBLBFNK87LXRjnfw/bWbsrtbaxesYQnnnqezrLPwQMvs2bta9BasDUuk+SeYdU77uPTF6/lhEVN9owMU/ZSdoWSufzvbH7G/+YlJ3QmX5zQFgerNk+PdjFVs3nzm87g4IF9jE6OUa9NU29OY5kucxcsZ82qpRSkT6UZIHXIKScdz9bde4kG9lKQkvleFioVmRKpNWOR5mCQOUqCKLMMjlQzDaNlQjPUSBN8Q5DUFYabRS8mKRgKtBZIrXF9iSUztqBlalotSZjMcBUNnQElRCbWjtNMnJq3IQf0ONBhQtHIYJgFX6MNcAxIDMHBMUGbp2mEgmIO5hQFcQLRdIPpuedSbpc0A7C8AkauTKNWpWApeo5ewNLjTqRj4WL8vl5yPd04OYNcR4F8WxteqYRtWyxwBZ5Q2KQUbIfFukaPmyASQVWY1BIwTUliWBi2h21mww+vt4u157+HDWe/CaWaHLv+BNadcSZ9S9dSKLVTypkUOmYzt3ch8xfMo69vHitOfB3ViYM0axHlng565nQjDROExDQEruXQClr4riQnE2JMGioLD8cwaZoGnpCUDMEcEePJmIJIaBcJvuOh0whDKF5JHBIrh5U0caRi8VlvBs/Hd1xEFNAMGtjSRAjwLJvpZg2R72J0325cx+SZrS9z+aXv4JE7HyB1c6hGDdMwMByHsN7MfOICHM9lbKqF61ocGauxZtl8Wo5Hr6igkgiTiEKYoANB1OmRDu/luZE8Uifc80pEoE0qQYveQoGpVkgjjEmFxLYEqU6RWmFIA2mY2CZoBEfG6jz89Ks8+fwrdFgTfO/aT7HpuZd577s/xBc+/1lmzenl7ttu4/s3foVKkmft8cfw+DPPoKdG+NMjj/Phz32R+//0FJ/98GdJwlF6l67i+u9cz6XvuZjJaoRKIkoFhyBK6epqY7zWZHDnfhIUpkyw0sx37JmaqjLotX2MnIOpBS0sls8T7Ng9zWWXz6N9Thcju4YIArI5vc5uOx35TBESpjBYywhUqc7UYFGiaaQSLQQqTFFKzgS6ZeQloeUM3EVheYJiSeNYgkJeIq1MwD27DDIF24A40mjHYGJCYxsCJcC2oBVkm2dhgu9DGitqVU1Hl0PNhFPnwNGfv5mpQ8NI0yJWBm9581nccsuPmTN/FZWpUaYmR6lVJ5gYP4xnC970hgu47tcP87vtCplUuGhlGy+NKlzLYM9YfNX/g6KoqeF98WDDYeOZ57Fu9WpWLl7MpuefxbRtKlPTNMI6Akmx0I5SEQ8/8BT/+PeXYsZNDo+32LJzL299y/n89ic/pt2AiQDKJnQ6gpIDZdvgqGJG1/YNTVungCR7s2oBBLGAGOoNjWEKohZIlZFvolDgF6AeSBq1DEIbx1limCGgkNOYliAMJVGqqccSlKbsSnp6s9dJDVIKIg1KQikHQQjEoJJso21LTbUpsG2BSrMRHTphuLCAv/7SVxk7fAjbdZBSEk1PkdguUbPK4OFRWs0qliVwCm1EI/tJohALBTrFECmeMJD1OoYEhUkr1fTmI0arUKkolOdR0xI7jZESlOvSvvx45i46mhWnnk9XW54obOE5Hl2z5jAxeoREO5iyRaHYhS09fLeE4xZp7+0jaTawKdB7zLHk9RSu20NYq1Dq76HY3UEURXT5FtgWaTOlaCqUMHANAWmMCywQLWY5CkOn2FJgY+FaEKkUxxJYhs3OlkanCVor2pespq2/Bz/XThQFYBhYQY3JBDwDJiYHcfwC4egITi5HdXiUFce+hh179nDJeWdwzz0PYxbLRHGI7diEQZjNyLUmVJrPfuHd3PO7F7HzBq7js2h+D2BTSitEYY7nxyNu3TlNsdTHzzfVyfkJd+9s0SK7IBxqCpa2QYpHI0lRSUKkM2G/Y2fe/MQwKDkCyxLY0sAUir0HJrjj/h1senITl719PRecfyZf/KcfMDU6zOy5Hjff8BvOf+PxbDjheHrNOtdccz1nv+FN5DvnYpsx3/nBPzN/4RosNcHco1ax/YVHeeVQhRUr5qKihCCMieOItWuX8rWvf5t5XR1UmppiLqEVZ/G/06Gip1wmiau4tofp5envarL71Rann7uAe7/8BNIQ5JwszQ+R0aiaKYxPZoe/Z2dkHN+Eog95P7s+V7TgyKQmBnI29BU0jiVpJZpGkDUcvgWOlSkQUhQqlKRaE8SSIIIghYIviGee11aYPV85GyYaGl9khdYRgkYdutoMRiYkhhY8NNTBV757HX+8/wnaymVet2o2wre44aZbScMpPNtjfHKYyvQEtmPiWQ47d7zMD79+I/v27SJfnsVPnx9iZKjJ9pFQhOr/QafY2Za/fcXCeUcvnTeHgckG7QWbBzc9TqlU5MHHHuXUU87hqs9cw23/+hPyngNI6q2Uv3nX2+lub3DgQI1mpQEo9o0dZujwAMrMrsK2gkqU2e8malmWcnteoiOBYYFpg+MJghYUylDMC9yiwHRBmGDnBNrKOIvNhiCxMltTRx4cQ+BamjgFnQgcZ2bRkGYnlGUKKjVNrZ5tm6QQ2RVaw3AlO8HGmxpbZjNO28qscFoJaoEApRk4ojj9U/9IdXwcpVoolUmEEiHp7OtldGSAOE1IWg3sXIkDz/0JvzyHZmUIZHZNJtaoIMBXEdLIhL5KSNqslI5cjGNCqFwG6g3s3vksPuciTj3jTXR19VCetxgzaaKiFtg5pDbwfIGhIgjrdM2eQ6mYw/RNVFwj1XW0MvAKJYRUELbwy4sxrIS2eX30zJrFUYu7KXb7jIdQzPtEBoSzF+P29dPQgjgKsA2bRrkLJ4qxDEmeGC0EUpoZ/xHJVKLYF1tIobE9j1JXJ0GzQb6tjVz3HLQ2aCRNpBLooEqaRMh6jSaC6kSdVjWiva+dVPhMtGI2rF3Ik49tpqOvjaGBCfrm9TI9Pk2sNMsXz+WuO56ms80DFbPv0Aid3W2cdcHZ/P4Pz7Fv2mTPUJVt+6d4w0Vn8uym5xiqGQgzA7lKQ5IkCj+XZX9Ww5BgJt/ctSxsU5KoLGDeMy1KbkpfWdOREwTKJGimaLedH97yGI8/sYlvfvZsDhwe4Rvf/gVz581nbt7jqmtuZHQ6YuWCpaw/92Qev+8R3vGuy9iy+TlM1ydpTdLekeNPjx9iTq7BeOhRKmTh7jo1WLBoNg8++CD1KUmiU4SSpDolSEBpie3nsEWCZ1pUmoKOUoiQHuPPTmOkU+jMiYdnaby8wPM0vp3NyCsBxLGmmId6JKkG2ewvTTRRrOjyNXPzAscVuJ5AuJBG4HkCaQhszyG1HcbrEdWqoNnMvNNKCUIl6CnC8BEYmdI4dpbTEitJNIMEnAogTOHwuMAQmch7upUydiTlpM9/mm//8E7WrF7J3J4SG05bwrbth3n+macxDEWjMc6ZZ7yBY1e/ji3bnmVw8DDz58/lJ7f9kCve/bf85Be3cOrx65huBozXGleFif7/VxQX9HXqN52w4uhqK8bWIdt3bOWkdacxWptiaGQUoS3OP3MdX/7ER/nBLT/hqd/9grbZC+nunUVbvo15He2s2XA0mzbvx5eCD7z/Cm75wQ10SI0voNOFKIZuX+PYAl9APVaEGoJmtjFXLfA9gW0KYpG1/qbI4BCuJyDNTqCCn3mf221JGGUGxmYkSFJJqrLvFbWgnmikIVFJtkHL5TNF/nQ9U90LE4outOoCe+aDZEsIIkFfGXxb0F7QeLagbXYv8zdeSN4xsU0bpJXNohoVrM5+4lbC1MAgIo2ZGB9Ba4fakYPZBy4KIAYtbXxDIqMGEgulFVIYFFRMLp+gIqgog8LJ5/D6U9fi5vqoRzGm49Je6MJ0psjPPYaUFIeQYtmCVCEMg+bUNKlp4Xg5Rg4P0r9kJWkYEakUaZcgmGByci9+oYyQglQbjI+10I2IYnuBfEcP5a4+ir5LqzKJZZnYbR1MW5JKlDJm2tRSk6Lr0ClSRBoSYmAg2BK7BFpkc0fDYvKVV1h0+kZynX0M7t3L1gfvpFSeh5szCbSNnzaYbMbUwpAUi3q9yVGzbGKjTNAKsNu6WL+gk7vvfYbO3nYcw8HKOahIcXhwmKOWzqXVyEAFtm2yZcs+1h+3hKmpiOFGwOhEjVaa0tVTIp6sE6iElpaARS2IkYZAK8GcQmb9DGOdzYClSWIIHBRTLYFva8LQoBm5KGHTaqW0+S5OzqGt7CPsHPc9dYBdL++ks8PhzDdewEev/jrHHLOKpbNmsWrDWu787UN86/s3IgnYsX0Xc+Yt4/6fXU/vvKXM6i7TPqeNl3eM4rs2YatJvlggDTXnnHcK11/3c4p+nmaswbBoJWCIhIJtY9gJqbJxvJiFS20OvBKRy0saRyYoWBlJScosMjgNQNqSRgvkDOtwsp5RmFxL0paDYi6bNwaJoKVBxDNX7Kb+C+s0FQa2bXJwuEEaCXQEFoJGKMi50Ghp4kgSJGAbgkgJPKHpKUE1/LelRsERODOYuUMNQV8btMrzOOsz38SII5YtX8aZSwsYpVnc9KPf4tgRlkg58uxjXPqxT/NPX/oY577+HJ58/jmKOZ/O9n727NlNzmgQhSHlYoGj58/+4isDY1f9T+Tt/7YoSimZ3VnW65bOZ3xsHElKmCQs6u+ibU4XtijhWwrfajJ74hVuvP4itj27gz2Do/imRuoYM0o57uyTUUeGmRys8dL+g5xz5jp+cffvsCcn6BKZLKa/LGilkvE6tGJYVBboVJDPZ27VaiyJYp1JFhJB3oIZ6yVFNxNw53xBkAiOVDLtYi3M0v08S+IIjetlbbvpC4JQ4EqN62QF1jIzoWikJflS5r1MyDpFz5PkihmUwpUw1QJbZN1chKZjw0YWH7Ma05AkKFKRIf6bzQZhs06hXGDo1VfY+tgmvIJLZfgIzaCJIQyEliiVDZXtpImfQsyffbKCXquJk2gahbl0XPJJ5s1ZSMPrxcw52LaDkBbbf/81nvrX37LjycdZ9brXEbVCKpMBQZRioQijiOqRQWpjVVABg7v2IPMOtrApFfM0Q4M0ajJ68BCmY1E9UiFE0qhWiGs10jShFcaYlovtOrgm2XIrEbi+gykVU1HCgcRAuD6TRp6CUMS2xda6QiqFaRiINML2bHLlNvZvfZ4nbruVMGiwe+ce5s0uI5MAnSqacUx1ukbQaiG9HKWiS970QGn2TidYs7s4f91RPP/0TvYfGGLBgrk0xkcxTItGFNDW1UVnZ4nxoQkKBYcf37mJ91/2eu6763GaUUJ7zmLTtgHOO3UBY0MTuKZFNYoxDBehUhJpMr8kaQmfetAiTDMAgiTBFgbVMMWSYoYlGDJSBSuRmFpQcmxUMyGsNqAeE1bqnL3xQtasPp47bvwajVjQNGKe2rSFG2++ibEjA3zps1ez/tRTWL5iBVf+7Ud46+UfoL27zL/88w+Zv3oVWkss2yTnOgjTZsHcxTx+zy8IgxDTCIhisIyUBIsF3XmSMEYVyphuTE+nRxxB3i/Q0xihGmaZ6ZaVgZr/XAiDWJOmglJe0l02yfmC9oKimJMUXMl0S9CINEUn0xcGQZYWaJhZo2ED9VrERJSFZBUcZubPkPPAcwRdZbBFNnJyHagHmVFjRV8mHA9CaLXA0IL+PCzsEtSGNf0fv5IOw2bvoWGO7e9g4VFFxusWd9/xK6xkmmajgmsmnLqsnzO6Wnz5tgc4bmEvdnk2l715Iw8+/HuKfo6oFWIJTb3VZN7svi+OTleviuPkf18UhRAsndurF3QWCMIWx7RZ1KMUPRNA/+iTz/Gut57PRGizNKdRjs2t9+xi//AQ+XI7aRhQkFAzujl5/UnoYhsrF5ucdUqJgVtuprD2TFr33sv8LkFiZqJN24R5ZSjnBeOt7A1PEk0pJ5jdC7lcJvOwbEj/P87eO8iyqzz7/a2149knd+6enGekmdFIMxoFJAGSCCIHG+RrMNkGbIzv5wz4ojG+zp8NNsZEg8FgTBCIJAQKIDFKaGakkSbP9PR0DqdPPmfHtdf9Yw+3fL+qr8q+p6q76lR1/3PCu971vs/ze0xNgqCaz6w7rtQkoSAJMjdOL4CRQqaF9CyBa0OUSBxT44eZL1npTBagZUYAkUIQB5mP2RIayxbk7EyP5VgSYWryVYM0BswsarXfheve+tsUJ9YRWza2loSdLqHvI+wcgd/FzFVQfszUxTN06y20Vkgdo0NNKgQkKZqUnEpwVJhtlJVA2pr1hYD4itsxb3gdVneBfreJcvNEzTaiMEr3+Ld55sgk0i4S9xNOPv4wju6TioSV2UVOHDuBiBL6tQ7NxhLEgnzBozU3y+riOYpDm6hNz9JdbeIOVOg0VrFI6LXb+LHCKg/TXVnA93u4novQJtIwSWMfK29RqAwytvsqioNj9Bo1FjoBfQl1H1Irx2yvi2NameZSC6QWCM+mdvE8Pc9laHwDMzOTmWRqIM/Cco2CZ9Fp+YhU4qIZHMxh5QewbAPZ69JLYWquzsEb9jMwUuHkM8+xdudWgvlVRJjghwGm7TBQybO4UGek4JAXBqlURFFMHKe0wgTDtLD8FvVEkurMNmrYKWEI1VKelf7lw0moy527Jkgl7a6i4BgYUjBQlCx3NX434m2/9R5st8DQxBhGovC7NW654To2jI0y0+ny7NGnaczN8fLXvIm3v+sNXDr+LJ/8s7voRqv8n7/3ET70/l/h+v23MLF9G889fRyrfRFnbBPTM02KRTvL4NEwt7CMbaRcevwRQsPBNBR+bFA0U4qyj1PdRhKHiEKHSj6lVc+aAd2tg1J4tiBnp5QGJK4naYea1a7EM7KbWxwpbAOKnkkqJF1fYxoGqx1NvZdlJQ0WMx2i62ZEqQSoDudwA0U3hmYCQZqJuXOFDK5SsiFUmlJO0OlC2QMRa5ZbghFP0+gJchI2DgrKhuZcTTDlSza9+o28fmyel7/yWtbtXEekh5mcqvPY4z8jWFlAGxFOcZAf//RRnrzU5MCGcbyRjRilUUqqzZHnnkNJiwEHVKrYmDOphwkR1l2pSr4URHHjf29d+V+6xHfedq1eaTZZU8yuMWuKNg8vh5m2zTKQcURhzUZ2TQwyaPQ48vNnuOPN7+LIs8/x3ne/k4l8g9pXP0e+38McKLGwGnLeGuKplsPffuFuvLBPXsKAFCRKM2gIxoqCAUsz6kEzyDzRpglaGti2QqYZUDZKwbahG4GlMolOyRX0QlCJJjAg6mexp8SCVg8GC5okzfzTQ/nspAtDUEpSymcJeVGksazMEA+wUhOsqaQU8gJHZh7vViCI0VRHNOboVRz4nf9JScRop0Qc1WguN4ksk269B5ZLv7HM1MVphsa38OP/+DRoQapion54+bqWIKKUchpiJ4pUJEgMAmmy7fmvhnKFcr7I7NkjFLdfh10qgQoRYcLikXu4dHGRqBsRqJRIJQRhiiVTRocGGBlw6SobTyQUpMNkGGLYJUpentRQDJclXmmI6bMXKBWGSC1w8jA3OUu1WCGMUqpjEzQbc6wsLLBh21UMjo/ihw06K6u4OZvuyjLG8EZypsuJp35Gq7ZKIkzWlPK0/AApdNZaWCbYHuvLgogU2wYAACAASURBVMenV7n6lpuoX1yk3VygUWtyw60vwPVsJNBabWPbFgNjVfYMSWR5BO2WWK7V6aQWqR/R6rRRS5PkyoMEnTYDawfwV1aZO3kJK++yaftGGvU2D/70GE7J4y2vvI7v/+gJEiFQWMwFEe/e6XKxFTLX1sQaWrFBVyvWV0qUrYiZZsRCSzFUypFqPxuN+Cna1GyqCtpKMrXY5TV3vJy+H9NrdpGWZsOubfR7IVOzlxhVile9739w8NqrePC793H48MN8/wf3UihCrxdx3a7tYBnouMeff+YbTE8tsbJ0lr033sxX//Ff6JoD7L/patLIpFyyGBio8Jk//whHf3YYp5wnTgVhlOCaFq70CZBU3AHW71UU7DytpRShUogkzunjDA1lhS2WAmFJWu3Maz6UF9R9CCJNOZfN94SGkcue6HNtzZgniGKNZwqkBa4BxSJEsYlXLXL6TIN+DFVXI3Wm0HAsTbcPa4YzxmejmxXSFEG3l2IKQbsvcSzNzhHNxjI8PgXtPqz2NbWRtex47Yt4Ub7OWLfBjkGTvCvop2XCX3sfzz09xX987YcUpeCpJx/m4A03c6ERobuLzNXqmEqQ0wmzScI7xmyO9DUIk3YEl1o+z56bEv+lTlFrjbBzd712+ygr7RajOYPJTsy+qkUzSAhSTa3T5Zdu3EPJLmGURnj/oX9gbanJXtmi8cAXOfOVr1It5ll2Ctw91edjR5b46L3Huf/wk1RVTEkKPCQ784IBAQM5gUw0cxE814EgzPJHTJk5UyCzHwWXf5IEyk6W4mchCKKMo9jqZRYl9xebtFwmufFyglQI0kTgx5mvWgADhQwpFsWZH1NKgWNmG7WJdRBrg36Q4qvMe9ruasZHBGYfun3F1pe8Bq0UPZUgQ0EvSEiFg99pEiYp7cUZmiurnP75YcxchV5rFZlmuTcqSUAb5CS4cUgkBQqZgSVKVcTgCJ6wWJ29iFsdQ9kOstdCqBjVjUnqR2kudwmURmsLzzQxDUgwaXYjFuo9okChkgiSDlVbs7UYAwmzU8sstn1cqQiTiGarS9TpIxxBtVRidjnL923UOxRL45SGypw//SypSug2OxTHh1m+WEMliqS9wuJynbGJdURo8HvUez0wJKbt4icJOVuiXYfq4BhnZxd5y1vfwtHDD9MPIrxcHr9Vp1QaZGF+HiUFBQ1rNo1S9Rzq9jBGGOCYKYFdoNtostrpUPRMuo0GfqS5dHySWJgIkYK2mDo3x5gXc8ftVxG3e5RHKpyfqiGFxhAxQai4/ZdeydQzp7BLFu3URJFSdgxq/Q5BmHnxrxx18JyAKLGIUYhAkqaa5b5BHKSUsQlThUGApk8QdEiXF9h2zUF+/R1vYe3VV/PkPV/nI3/wexz+6cPs3L2Zb373+/z27/0xb3vTr3P/kZ+zstLiisp6nvfKO1CdC9zzT//M/pfewYVHH2X9zbdw4eQclYoHQuIKxfv++A+ptc5w+GencE2HnAVKC3qxwhaauWad6/eO0gkUcaCwRLYBNnIuUtmElsYVMXGisUT2vWr2NEVLs9oXmYhaCCxb0w0F7S5sqEosDZHONtiWmfFKdSIw8lYmWm+nVPIS1xYMliXSgG4i8QqC8WqmCS4Vs9m/H2lKeZM00aytCio5TRDAQxeyYLXTPiwlEu23OP/Y03z7eIsLQK5YRukEo7nM7Bc+x1i+yZv27+KFr38NG/e/gqkT53nv62/mhw/8lHo/xpGCgpny+hGH76+GbHAlrVCxd3SQe5+bElEU/ddnivO11qGXXLH+ruWej9SwpSh5dDHkQNXiQj/BcSzO3neYf/rRfWxJT/ODv/sTRpcvYK6uMFIqcMQ3eds3T/G395/m8IlpLs7X8VRElGaWrz4QaMFSoKnFYEQaC6h6gq152DUuKBRNJJrW5SgBR0IsBI6dWedQmZXIKxqUyxkdBwS9OFvCIAVKCsJI0OyD0Nm1OJ/X+CGs+IJmoPEDQaQElp29cRgCS2YRBiUXygVN3pEoQ+AVNcIWRBKWrFFueuNbCBuLhLhIA0RpLZd+fi8DV93MiUcfpzw4Sr+5xMp8HbQijn0SPyRNNSkpaRgzrPv0sUgwsDT0pWTtVdcjnSKkPtLxMHM5ajMXMYM6UplMnThMJddleqaPH2sMYpSWeJaVnRiujWGYxFFEN4qo+5KZVZ9jl/qsKZsc3JJnarHP2ckl9u7byeLCKk4lR9DyCURKGiUsz14iVyqzNHeeTmOZ7Vdez8WzZwgihQ4CtOcS9vo0GisMDo9Sq9eoFvK4pRI5mTm+e902ZtLn4CveiZ04zJ46RctMedUrb+Zn9/6EIIxJVIrEIFaKhMy+NjZeZazk4HgOrVCQBj067R6eIel3u6RxShRF+I0unuVRGq5Cr42SGaQ3FAb7hkPOPDvFqYU2z56e5WP/+B6+84OnmKh6OKRMdkN279rAo8eXKOcNVBLhp2AKk9V2yrphh8RICBPI2ZpWKFCxwjEN8iIl7+ZYs6VKPwpZODfNvhtv4d0f+CD7XvRyjjzyMP/+6c/wic/+G6PmKN5okT3DOd7151/lkUee4MKZaU6cOsudr7uT1FU8ePRpGkd+wue+fB8Hr9+HsIpYWpKKPo89do6JkQrlkkelXGRpucE1N1/F5z/6GYaLA5SciFhnc9tUa0gVIoAN2z1OPuOzZ1cZx4v5+ek+cp3LjVdUWZmsI3KShTZIpRnIZzBi18huRVYhk+3YRibw7vQ1S4HAvTwTVDpjiFZdiH2FJxWRzm59JUsT6oxjkLNT6g1YOyLQholZyZOmFjkTHKFwTEGUpCSRoOwKSqZmpiMY8LIiqZF4Al630+HBSyt84oEzfPKRKY7WY154/S42GAKjt0Tr8A8xnnuIN3zsAyyvwL/962fJeQU8qVljCY53YvYPWqyEEsdxOLHU5fTswqH/2kzxcjP5y9fv1T2VsrlSoBtFLPgpgxZcDCUVFDOLdX7nH97Htgc+z/LRZ7jt1ddQ6fv82QOnecc3nxPfOT5/yCa9a+eaAjqI+L1bR7j/XJ9nH3orX/72aeJAEaAJEORllkPhGiC0JNSaRk9jpJKCl1IuQqsryLsi25QJqOayD2rekuS8FNuU6DSjc8cRDOQFeVOTRAJpaEhBXCbzJHGGG4tD2FzWbBzKTsudA4KpVWj60I8ykatpQaoESy2NcAU528LXLo264M7P3kN3cZ5YZye1gaTbWcVvtjn9k/vZce1Bjj70CMqy6dTmUYnCb7fRpsTAwrQtckmIIWy0yOIGEp2SGxrBHhxDWnnioEuuNEyn1UT3O9SW5on8Ht1ukw2FlFPzIY4hKDk2YZpiIMEQRGGIKRSmJZAIcpaJZRl4jmCmEXFuvstI2aACPH12nspQkeXpWRwvR2OpRqfdIz8wSqdew3AshLSYPneR0vAwxfwA85emcXImjXpErjBIfXWV0mCFdnMVlcSEPZ8oVgyUi2y/6oXMnzvGmdMnMquhBCNoc3pyCUMLpGWRpimkmjSEwcECAyNVJkYr1NI8RdFDIen5mQo46vboNFZIMEmSiH7Ux0ojklhlp6EpKKIIdMK1FZdjqzEl1+Q/vvEYb73zJq4oCpbbPZ55Zpr3/+5rOP3EWaJOB7dgoOOICIkfCnpByvrhPFKmxEowV9fkbIe8JYlEgiUdtuy7lvf90Qd5xZvewUNPHOaTH/sHzj10P88+9xiTF5f5i7+8ixteeR3f+eQnefMf/A3nTjxJffJHVMfHGR3bxunTp9gwOMKJk4dJtM3+A3t482//Cc/8/Ajrt2/hPz7/FXbu2MpSJ2XDhmqWfGhIRibG+flTj9FZ6WOiSIWB0hFKKRzL4NzUCi999Q6ePrrCxFAOQxoMDVq8+cNXcuwfngRT46rMHrvSE5gSTJkJtpHZVVeHmR/ej6CXZA1C3YeqlxUNQ4KZF/TDzELY7GUb6bwrKeUz/70jshtW0QRLpPR7EZKEVGkSQKWC0ZzmyZnMinu6BvVAUPMFdZVBbG992RDn82vJqw4TgzlmVgKarYCPPzbFl5+eZ2tBcvUrtjNw5Vr49BcYffEuvvbQMiUR4KaaWEpKOZMVXxBJky2FHNORQCh1V7PvH/rfFsX/TAaeqOb11i0b2blnD9bceWxpIGVKogVCKfJJRGX9KHcdrJKenGFoe4F/+sJh7vjCMfHwdPuQ58jNbz44Un/FFTm2j9ocXbE4XQs48/Vf4fc/9F3OTHfYsWstKgyII4VA0tIZRqyhwBOSnCNxPU3VgulG5pNcaWcJdY6RvZmJFrSDjLeI0tiAMCS1liAxsiIWxhkgIok0jpHlR+RMQZRkonRfCWo9wZqqpuZnlsGik+WkrPrZBjpOBQMFMC8TNVQ3YstvfICB0bX0W/MIwyXyQyJhQdwj0nmCTot2ahB023SWa+TKVeq1BZRKSOIAy7DpRj4kKXYaojFIUeQKebxN++iuTGGXxgiDCB11CFoBy7WLJFqSJjA27LI01aBsC5qhIDU0hhREcYwnbQKVEiiNUimFskuhYuMIC5UkSB0TKpCpZMvGIrrRYamr8DyXVNkY2sEPQ7rtJt1eC4HAyRcxczZRL2J1dZFcziKIFUHbp9f1yTkunWYTpROSOCHsBYyvH+TgC17Nw9/5Ft0g0/s5lsCPY3aurXJqchnDNLEtgzRNMuqLaRDriD2bRslXyxiRj1KKJE4xpUmnXodU0mw0qVYG8f2YnFAYSYRyXOKgRaOdLbo2lQpYhslT802qjolpSY48eo44Z6GFxDFyfOMHR3nPH7+BrVWTI8fnwHZZ9RWOsgg1XKpFrLQ0S01FAYHtSOIk4eDtL+P5r7qDtTuv4hN/83d8/5tfZ7Dg8lu/9mY++/Xv8+9f+zLv+s338+GP/BUL547hVIe44QW30KGMURhi9fgTPH74u4yMrmd+pcVrXnYH//zFf+FT//o17v723QwWC4Q6ot7scNv/8SaeeeoYUT8kSmI2b92K34+48ba1fOkTd+N5Ht0oJlaagp3denKeiaFd1u3OY3QiOiH045Tg0UVIatlMT0EcQzn3i/RRAfJyzhFZ0TJNgVYZDcezJEVbEycCO58VxSEXun6WtmmILKvdEJkBIp/PiEWeJ7hyM4gQSMBOBWmSEvQkSyuw3IFGW3OxI6lFAgyYV5ptt25i5mKTH8/0+eibN/L0AuhOk7fdPoEKI5baCpUoPv/MMg/df57NqsOmV+2Cb9/Pa373ZfzPv72PnWuLmKaFbWiKtkFFwFLqcMtN11GrrXBpqXZIa/H/JaP/505RCEHeMfmjt73xrjXrNuM4Do8vNXmxk1BLFCEpuzxBI0j5l/fuovHwWZ6aXmL/Px190bcmO28p2uKp541an5r19e/c/dHb7xoKIs4vdrnzpp2MbRrlgcPH6AuPhbkOv/u2q/nK/Rco5V06UYzrugzuvZr84jx9DQU0ttLUe4JaJ8si/gWCy3EFOSubE1ZKYLgGaQqhllhGBiZw7Yy6nURZ8xtHGYOxbGe6yJID66uSteXshJytS+JE0EpSLrUlscrwZEVHE0SZ/EcLCYmm7+S5+k3/g8bMKQzDIHRKhM0GzXZAGvjMLkwzUCkwef4ihilZXZxHxxFhFJKEfUwjhzQk0pQUEk2iE4Rh4jouKlelsTrD8J7biFdn6fkBIg6prc7Rafg4pg+qybil8aOIUJkkqSJVEUXPoBMI0jQlVHHGqxOafN6G1CRXsClVrKwrt/P0gz6NfsrzD27ixPkFkjCg53eJ/ICQhNAPMgR/HBH3eyRRRgDSCqLLLotcsUISNImDHrhlXEvSWW2w5+arGXI28/1vfpnSQAmhJUJoIq2JVUzZlVysdXENiSETLsdz4xiCsZEBSkNlIqWwcy6RMAl7TcIEoljSC2OSKERFMUG7nc2BnQJRs4bMD7Dp5tfx3JOH2TmYx1eK5UYXaRiYwsD2TJrNiIWlLl6xiOF4/PCen/LTZ5cQjkOtFSJiEykt7DQlZ0lcIwNlWJakF0TsvuE6cqaBSgWf++u/5GV33MAN+/awa+dOvvSVL/CSG3dxYqrB3V/+Iv/0uX/lLz/6KVSzxvNuv5WVSwskUcyaPbu4754fctX+fczMTlPwE6rDFb75wx9x0403s1hrsTwzzYad22nXV3nu6BkGRyuYVoRjWjhWjspQgS998StY2kBphdISmWaqBikl544H3PmuMb57T5OhjTmGA0l0+iStriCKMtiKLTOqjSGy8ZJWmTbXrkh0Jo4gDDMjRTfICEuFXKbWMCXYOYEpBMMFTSvU1HuSOM3iDkxHU7WzxMmSBBWCSDNSt6EEq91subnQsOjs3Yc5O4+yYVmamIbJmi0DHDgwzEq9z5tes5EnnmuRmg7XrUm4bd8YVw6ZnJ7t4bgm55sxnzta49h9J7lt7yiDySK3/9pNXHrkHMW8R1cLXDQFLIKRdVw5UeHGPVdSKbp3PXX64qH/NdxN/uLerLXmbz70O1qZOW4qxvjnj8PyEonncH3ZYXfJpRQp/vDGAb7zlaf55tELvOreGRFocf/OgbwOQn2gWLJ4wwFPzz9xnmdqEduHDX7yxJMc3Gnx13fPcfzYLP921wuwen2kFDzx+TeRGDbm5k1c8eK3MvKbf8pqrDnfh5OB4MyqwJAaqWAxSSnkJVpnVr4g0HR7QKxxbU3R0SghKOU1XSVBZVvqOM6wSZhwdimTAZxYEnzrjOaei7DUyvBEwxXNmpJB1dXkHFhbBq0lE4NQykksrWn7msLQJsKoS6BSat2U1A+pLc2hFEydP0tzucZK34VY0u12CYOIdqeNSMGybVKZEkZ9jESj4iDrElWKHyc0V5cx3TLtS8+ytLSCmQYs1ZZoLq5Q8FLGPcHugQq+3ydXGmGkmkNpME0PmaTkrJQo1QghQRnoFOLExCtabBxwUQIqE6N4bkKlWKDfDzg9VWPTYB4lBFGcUK6MkHOrGFJgmiYqSfEDn5QYFce0+j1aQYSIAvxeE50rkLNNrKjO4sVJ3v7776UsR/juj39ArjpAqgximYXKS8sBbTBT7+NKg1SkeEhSYeGYFqaXp5toHCNFKkUOjZNqQlnA0ilCQq9Vp9Xts9psYxkCE4Hf74Obw19eZB0LbHn+qzjjrOEnlzpE0iQxXEIEkTLRqSJfrmDky/j1Gmu2bcU1HMraYsguUJAxtqEwnRTDdlGpIu+6mKZFtVrAEiaua3Hvlz7PK3/1l0gWl5mcvUCz3WB+Zoktu6+mvtphxxXX8bcf+SCf/+RnOTnXwBEBkxfP0Eli2h2fIa3otBtYpmBycZaX3v46nnjkEfZfc4B2e4U4DenMLnHvl/6V9VsniHtdCrkiURzRai4jwjFueul1dLqrCK2xDUmks6tvqjTlXMiTD0WMD1vEfsRsr4vteJgGmKkm6WXMgDjNCmQ7kOSr4OYh6mYzfpFkwWZRlBVPx4RumM3vJ4YNtq8RzPc0hbJBxTVYX4bxIuQKAi9Tr9Fo6ozmnWrCMEMCdk3BNqGZsjbR/+DHkHvv4OENG9lrwCt+ZR9rD6xjcb7B+95+ENcu8Prf/AmLqz3ecNCjJUs8cnaJazbludjW/PoukzDVSMPgezW483PHeexr59n8vcOs2TXBZjNhg2PwkqE8D7djahdOsdVQFOtzvOX51/H65+/T+jLK7xfRlwaA4zrc+8k/0W9wNMbKJP/+8FEWFuZYaXX562eWt7z86u3vf57oMm0IrFab1U6ffzzZZSpID+U0XPjI0F0vefnVd91Q6fGT04IoUtz5wiuYaYYMjRQ4N9fnDQfX8th0yInZHivLKxyf6fOH79nL5+7rImTIbS95O1F7Fe+6F7OaBNQuzbBJpmgp6KSCIRdmOlmWRBwLokQwmMuoN0pLun5mO/JjSRwIBJIgTMmZIF2BUmCb2QmXt2FLVbChBOHlUFglNP1eipPL8GSdJFvm+CH4cUorkPRjjSgXGd99Nb0oJA5D2ssLNJshRsFhebXPyNAAUxcn6dUXIU7o1ReJYp8kAUNK4jhECBPt97CkxECgRRYUHiuNlfcIOl28kfVEfpfZyTkGyxb7tw5i+oqR0SrtdoeCY5I3I/KewXS9h6mz16afalIl0KRIYZKkIWkAqe1iGJKg3UJ4eYoFF0sI5he7bBkSnF4IKHgeQeIzvnEzM5OnSA2T8cESuZwHWjBUtnC0gVbqFyHSmWdVJywu13j3Xb/L5IUFvvHVrzI2WMZSJlJGxMrIOkJhEqmAKNEZVNU0KVoGbWVgGJKibVCuDjNUtHHyBcK4j9YJdq5IkChUqGg2G1ha0g8iLCPNgAVaYNoOMvY5d+ESjplDejk27b2GdVt30ug2wcxh5j1GhwcZ27Kb08eeYN32nbiFIpZpYLoO5eowWzeupzgyQRCH2CRI06Afhhgyey3HJzYQqy4nz81x9bYJer2IXD5PujrFz0/M0+21ufHGO7h08RSmUHi6w5kLlzh838O86Tdfz/qyRVo7y5d/+DT7912N78e0Om2mps4yNDTMucnTlCoTtFcaGGbKhZMn2Hv1Ncws1RgYrFDI58gVS2gluHJXgS9/4TvkPA8lYkRqICVESqNzmsa5Pu/4yB5OPNGgOmQTxwZl3cYrZresnCUYLGiKrsC2LltgRcYfNYTAcwTViqDgSjxHUPJSBjxJrmBgF0w2bKnyoxM+ti0YrhoYtiRNIQlTbEuwrgqOKZitg4gk8z6IlmYxGOQzm1/O4EvfyBrDQUVNFhdnOOInXLc5Wxh+79E5luYW8RyL4dEhbpkIuLAaMzExhq0Nwm6Lrz/T47eu17z2jTfyoR11Hl0QPL4cM2gkDDuScVvxbGCw15H8NHD5+pMnhWtw1zefPEFfadzuKh969YsobJu464FHnz0k/nNR/NN3vE7L0yf41IOP8Vc/fJKjs7WvH55r7z7f6B9SYdCYKMi7HKFZWG0g0Ty+GnHvUiAGpKgP5sRf3vWDDoPzM+zdtwk5McDHfzDJK/eWkZbAtXPsuGoTz5yY47Zrd1DyUu59eJIJT/L57y0QFEqYaEynQK44QMkwGdl2Letf/GpWWm1WLk0hhEZrga81W8qCahnGxzzmVhUizUCpQxWJkiaBnxLHKY1OCpe/MDrNZiGOmQlLm37WQbYVlN2M4pF3Mh91kmRCY1dkNkBtCrhsCxwowNDzXkixOkbj0jRxrwV2jqVnn8GsjHP6yYcwLRtLQmNlBYUmaneJwhADTZIoFCkiTXHsHCJSKJFBZlNpom0HGYPh2HTby3TaEQOeyW03rkVYHmNDgxy8aQ+dlSaluI0fdik4grINvUjQVTF2KnEdCFONThNUalAdqaJaHUbXjhMmkmpestTuMVh0WO2FbB0oIKRgudkl7DUpFMvc8ivvoj2zQtmMee2LtvCzo7N0ugFj48M4wqQTKRzLxBMJcRiy/9abmZgY4TN//2nGh8cxtEBJSUyKbWTZz8I0MaQk6PdRhsCRJqZl4MeKgudgl0o4Xo41QzkCFZF3DPrYeCKm1g8whIMmolHrgEqxXBOLFJ2moFI0imatxc49uzn21DHQBqlOcXIVgiBk/Y7tmNIl9rvUVhqs330lQjp0V+cwDYeiiFHSoz5/AbtcpVVbAcPBtVxUkuCVigwOlwg6bZZbEXvWrSHVKW61wk8fe5a3/vovcX5ynk5rmR1X7Kbb8UmdEjlbsrq6wHfuvo9zx5/mwafn2JjrUlq7g1a7x5qhAbRT4Motm/nAn/8Vf/LhD3Dx7AXMJOKK591KvTbLpckWo+tG6PshEigXHMZ37OZrn/ocOStPksrLgBGBaeaQRHSDkPGxIksLPbau8zjXMCl26hSlplSUVL0UlWQWWtfOSDmek42phNJZ45Rm+C/LIYOxBOAKTRKn9DoJ7V6KY2oKTvZcJZKBMlTsrMieqWfg42ZD0296nL3uLUy99nfZViowWCxiux6P//QeZBITSZsbdpY4/MgZ8iLGdvJcvXs9144LCmvG0Fqxfc0wtYUFWpHmoTN9/ubdB3jiwQv8/SNNppY1poBjnYSXri9wvqV4thlwzUiZP/7RM3/VC+L7F1q9Q7Wuf+jC7MLho9NLv3bkuTO88LoDPPTc2bu6veDQ/1sUHzpy8tC3nrt06Kn5xqFWog/1lf76f75jPzHdONTP5e6yXJsFBX93bFWg4c5rK3+50gq4YtDiK2cV77nGwS86fOqHC1y71uLXPnWG2swcU1GJpdlLvPPNt3DxqVPs3b2OdmSw4ExgG4Kw1+Pm217DxPhmTk6dpWBLBnMOlSsOMHbnb3HpoW+zlBgMGwkzfThTF7Qaig0Vg1CnNDqCkpN1h42uoudnuHUpMqhENxCEiaDva3JWhlIPInE5lMukMlwgUCmVvIFjQhJoigWBdDSGNkj8zBbY68LIVQcprd3Cme98izDv4poWT5w4SXPmLF51mPr0OTp+j6DrI6VJr7OKZTrEqUKnClNIpGVmRO0kAWmABlOaCGGgdUIUx0ShZqWxwvU7ilx38ABr8yb7b93H+L4trNs5gkzzjAwPc3ZqiWYoiGPFSN7GTyHVJo5WhFpjC420Bc0wwRU+c6tN6s0eDiZRrJFK0OwqBvMJ040E23BZWb7E5FM/Y++NN3F2qYZbq+MVTJbbKau1Onv3Fmm3Ne1uBy0zmMCWjaN879NfwypW8SwH03Ih6WdykVQhLButMj1pPwgQhsyiMA1BrAWubWI6eaTuU64UMQybxdU2ruehIkUiPJRt01uuEaca1zCwTBPbtemHCiFMEkNTsiyePnGSA9fvYfnSHO3WMkG7CVLQqPdIel1iBL1Oi9zwWhbOnuS1v3w7y5caOL0GjSTGHh7Hki5J1GNifA3oCMu2iPyY0fUTdDs9DCzWjA+RCAlEJFozHHXYtmUtP/juA1x94BpWFk/S7cLI+BD15ZgX3fZ8vv/o04ggYs+WK9GGYMOGNRw9dYGJteMYmYW28AAAIABJREFUcYSdz7H3moP47TaNZpvtO3Zw8akHGNmxlycOH2VsbJCRkUGKA0NsW7uBB448Tm+hTtHoU3Ghn4rsQBLguhY/+8kcr/vVzdzz7Rmed9tanrooaJse01MRhZddT3FmmtiCKM2ssa6Z6WWlIIN5SIEfCQyd4b8qVdCGRAmTejNl0FV4RsYgKHkGQ2OSskwZKGksLShVbI48m2N1dDerH/oarqkZswXNVhNfKUoywi0MMXnqSQrlIj862uF5uws0fYs7Dw6xvDzDmpEC33viPA883eITP5wkny8gu02eWUl5486U//t7S8y1BfuGNSuJwR++oMSHj4VsHMnTjg3+4tgSK83ezf+5pgWKyVU/PvTscuvQVx944lDvckH8L6LDsvv22aXWoXP1fu6J+e7NOk5Z56Df/PYdPFVL+eCVIfdfSNkxCrPzcP+ZFn/66m3cusNhsdHlwHDAn97bYHlqkg++7xbueWyJH8/lyefzqKDLlTe+jIfv+3eOHn+SN960m6HhQQzTw9Apbr/Dxhf+EjtueSnfv/9eekGMqwVCp9TbCiJwXUGjCypShIZEkNIONG4OTFugk2xxki9IDNck50lM06BaFRRLBmnsY+iUpZYiDDSYmlBnp2Kvr/F1tpE2DTAHx9i8fQff+Pb3Ob24Qj7scGFqifZsA9NStPoJsR/hd7Lg3ChKsq21NkGnCCHRWmGZkjSIETpFmGbmdjFdLBM6fo8wCqhUHF66fxPj42U2jFZ49vQCP/vCv1Osltm4fZjVWo/+wjxLnQTLsxh2IoJY042SjH8nDJI0zfKqpSTQNqNDJTy7QM7UaCWRUrPS9BkaG6EiFLUgxXVLpLbF4uw0XpLwdNvmtq0O5xZ7aB3TWFXs3lWh19cECmI/Yte+7Tzz3BlyXilLVIwitMxiVU2REsQRAvBEiiM1XSUo2ib9yyJiSIiVYGSkSqVSpt3LXCQyQxghVYJNzFzNJwmj7A0RCYbpYHsOnVYDK4yJDAutJe1exJot48xduIRhgk4DIEBFCSqIaHVWibodXFNz6qnTbBkuUd25n9bCIl5liKWLZxhfu55ep4mdy9HvhKhUM7a2QhKk1Fo1dm3fRqO2jG05bBnKUYtsao0Om3ZsQOaKJP2UVrfLxo3jPHb4AQ7c+gr2X3Ulx449yfnFGm/55dv467//BLuHTM48e5T8yBa2bN3GY/d8gy3XXEttoU67scSzxy9y3ctu5fiRS4ysLZN3bUwzYnlxlergEKcfvAfbMklJMNH0lIVWEalOQcJVG8oURxxOP9uhOlDkYk3xvrsO0n36OHG3jzSzMtBPTMJY47lk8QIItJnN3DUat5DZ8RxL41gphrRYt8mjWlSMDCmKOYtcosmJFE8InjqnmfQOYLzv45SuvhGvuYjl5kiTgJfu38ndX/sM09NT9PwOlaFROu0GA9UyT092+PXrDdphnyvW5fmNz06yf+cYb3/hONu9FlcOpzw9m3BqOeHgiMmnj/rcuAYemdb8y+9t5M++ushUM+RMLebJ2aZotvuH+G88jP/OH/uxuj9KNNvKQmsJjz+8yB+95Rb+4D6fd+6MmFvo4rmC+y8EvHxzQkrKwd3rSVVEIzD4+ek2Dx6d5jtnPcZGS8QqYnjNVpq1efrtJoVShcLEDtoLF3ntS27CsByqRQ9DC0yRcvbYExj9BtKEoZLJSMnAcGGpl1Gz+0bGYxRKIs0sX8bR4NpZtGmaQjmXYkiNiFOEzoKdtIJOCEvNDHhQsAVhH1xLk89Lyl42jA4C2PiC25COy48PP4aKFCMDkunZVbpGjIrSLHJAaVw3R6/fQRoOSpOxY7QiTVOENtAmGFqDtNCAtDxAsz1dIlUJjVCyc9s69qypcuD2a7jnSJfzzT6N2OJH33iKwpphrthW5qdPLjBb62HbLn0krTiPEA7aBJOEWAuSOAFhYKQKv5PQ73VJ+orUUBimhWdqZufrVEoWpJqu38Uzc4DEiiOkTkiClI0TeRY6kjAISVLBSKlAHMU0CNm3bYRTJ2axXZc4SXEdmzDoZy4dqQADYRg4jkvRktR7Pq6QGLZJpAU6TSlUBli7bohWPwJM/FhjiZQwgq4f0E8lhAn9bgspwCt4dEJNu9fHlCZhlOJIRawVS/PzVAer2KZB0OkR9iOiro9hO4wO5GgutfFkFtpVrRYpbL6KKO6zducVLEyeZd1V16BSjSFT+r2AYtGjUW8xsX6ETi8hjWO27NjD7IUzbFy/nsm5VZQwKFZy1GsdXE/SXF7CLRUoJl2eOX2RzduvIO7UWa43uen26/jOt+/HBl755t8gThO6nQYDtuBT3/wer33Fy5lfqpFGXWzPRbZrnDs1xej6dYRhyK4du+n3Y8ojBf7t019iqOSRaoHQGRwlvXwA26bgseNtNo9V2bQGlla6DJQcylfaLH3nNMIShH4WJcBlFUAYa9JfwJjjLCDetgQq0AgjG0PZhsQsuyQyh6cjNhYUuYLJwJCHaaZ0exohNGfGb2b7FXtxbRsv77Fvx0byRsyPf/Q91u3cT2t1icbyDMXBtaigSxxFFEpF7nmyxsuuzfOuf55EJim/etMooR+SKMHt+8f4+mOrYBhsLyjCRDGUk/zp+/dz973TzNQiNuckFzqRiJXmv/uQ/91/EGhOt7T4+9eVGB20WXnwPn773XtIUvjKacHvvGcNgzmD93xjle8912P91mGGi3m2rSlw5YjmqRWX9RNlkijBNk1yOY/a4jSvvfNt/PNffIBD7/1Vco7kD/6vD3PL1RvZMxDjzx3jG1/4G/pzkwhDIFLwg5T5pmKxpYmVouuYTLZgJZBoqXEtgc6OO5TQFCtZaE4qBYGvafiaVBkkATT70OgJKrkMsulHGRk4QRIEmnozS6ByTJg+9RyEPlpDEimC1KAXpZRyw5k8JkkQaJRpIDExBQilUGEPEBlzUStSPyQOk2wehkmEYEB1qdJib66L7PtsGS2zdddavvjpB3nxr76AXDvg3Nl5RnZt4PMf/ya5LVu5bsNaUpFQzSk8baMwSKMuOrVQho0jsg9wmkT0/BA/jkhCRScIUYGi40eZn9wymV6JGay4eI5LFEWg+rhujjyax2Z8xgo2g26EYdssL7dpxxEVx2T72ATzZy8hvAJSSBypiKKIVCm0SqhaAkNH5AxJ0m2iE0XZuJx3KcDEwDZMorBDp+0TBzE9P0AaEj+R1Ftdom6DXi+g3fUxHRfTzLI9Cp5NPufR6fXwjIBWPyYOEip5h4WTJ5k+dY6B9eOUBobI5V2WF5a55VV3sKxj3JxDreNTKY9gWCYqqHH1ra/hqpe/Ei9fxEKQBAqZCvq9CMsSRFFGmxZWnjAKcarj5AeqBHFKr9uhm2jWr1/P0sIqTqFCKkxarT5b1ozRWphhevoim9eNcmm6iec4LNW7XJi8QBRaRCqhHyp2b9rAzMWLqNDHTySj67Yy0/JxqwXanQ6DI2PMLs7RbcwxsW4NA9UhUsMgiAW1AGKtkdIi1VlwmWfE3P/IBe7+cYu16zz2HvD4j4/P4TgCkaTZ9lqmmFEWfLWwmjUAcZJmQA8ro1GVhwXFYYF0BZYL692Ai/MrtP2Eo1MCpxXSmGuzMB2x2NE0lWDxvq/x8P3fwKyf4dX7RumuTvHxT3yUN7zujfzRu+7kbW99F45psjL9DBt3Po846qCUYnTdGH/0xSXu2AgBsGHE5fotJVaaMa/62EUWugbvuH2Ic4HJck3jXneAD/3rSWYXe7x6IxzpaZHy/+/x3y6K+vLvV32xLX7/gODrk5rGV++l9N5XcF3Z5DXvPc+FH7wKK3X5wfEW2975IB/89hx/8vaD/OiSZO3ajYRJTKe2wMSGPUxfeA7btDj52L2cOfo4nXPfohuGhH6XYniRS488xJNPH+X0M0cJpCDVktVUcN7XnOyldAo2r3r3izlp5bj11dfy6JJioGwRBil5N7s+y1QiQk3Bg2ZdEymB7QjaKqWjsgXAQD5DHaWmgdYpYSKIQ7BLMDgqMXJZt5j2AgY3bWbN+EYUmr7KqCn9qI+JgcQgSRJUr4shDUgNLMtEmL9AhUVIYeIKNzNc6wSERiQxG80akZK0+glrJgqsGxukUBLM9Ewe+Mw3MYjZf+BK+o0WE0WPL33qftbuHGLrqIc3OEZ1uMjmQge3aKHSBENaYBukUqLQaCFIVIqd85BC0o4S+j2fIIwoOAbFnKSfOAxXcvh9Hz+ApXYTLSQjJZenLrS4addWIEUbBtOzdfraZGh0gIXlBOv/oe49oy27yjPdZ86Vdz5hnxzrnDqVk0pVylkoIUAyCBswwmCCUzu0m7bdNhhuN3YbZxtjgsFgcpSQhECAAighVamSKtepcHLcOa08749d7nFv/2ybMdzz9x77xx5rffub3/e+72NbKBGRNEFEHulkCmkouh0LJ27/AfR0pak2mhhCx5QQoWGZBqHyiRsejVaAW28SRYI4UBiaAcogDAX4EXGs49UaKCTZhIGHxFABuWyOViAwkyae8qjVPSqhwsilOHvsDAuL89TdkK6+Tv7hY5/jaw9/nKZucN/b7mG5XGDD7imue+DXWX/pCZ774uc49fTjNCplkl0ZhK4jpEJF7WffsGx8zyWMFAYx1ZpOMtcFysCvGVyYW8f0QyrlIltGMxydXmbTpikuzl8ibnkkbIdXfvoSb3zTbVS8JpfOnEGmsySyYwjDgs5hgtZ6OzzWTKOESeP8Cd74ljcTx4pU1wCWncLuGCBuKG5+y/2sVGJqgUEsNEwRXxbXKqSQCL0NJ4vdOo98+wLf+NIq996Upex0EbUUgQuNuiBWEZ6vSNvtSLAgEEQxtFyBpYEdQWEO3BrMLCiOXgjJ6jb65m2sTGzjDw4lOVc1MG3JbEExvQqRHnPhyUc4fuokKwefwfaLhGHIwvmjnH7+axz7ycMYWoyGxamDjzG+5VqK6wsgJV1jYzy1kuK+vXkWSjFv/ptTvHCsxMWVBr/39l38/HWjfONAjV9+yyasE2eZooWuK750qd04/X8r1s/s+vy/n6+cUx/+xK3JDz26DKPzi2z+9XsJDp7gwoHzfOmj+zl+cJ0Ndkgc+vzul86zYfMm0Ax8t87ua17D6tJFNE0j1zPEpF5gV/k8s26NL/zgGB/57XeQrdZ58fQ03/7+E9Tr9TY5LFbEcdtfeev143zwt+/CMRTveP1VrNcadOUzfPWFJZSm0YgVfggNT1D12p5Ny5bUPGh44IVtfaYXKmq+oum1N29ucFmkailcV7Q/X1dUW5La6jLdm3fSkoILp86iaZJG0wMFli6RIiaMIizTJooVQtcQUkcgEPKy3F+FqNADJZFoxEIjGTfoM6u0IkkQKXJX7Wf7UBZhZjl0aol0o8a2O27ie19/DGEZWKZOtbzCrXffyJnTK5w6v0IqYdLRlWR5vYnSDWJ0lIrwvAhd0wn8+DK/N0ZoEAaKPfs2sLRYQwpBtVpHJBKMTPSx+9o9TE0Okk6mWCmtI4kpeRY9To1CTV3WtymqhSrZrg4W5uexDbPtWMmnma24WI7BcEJSKjfJ6m3AlpVIEHkhhhSECHzdxNJ0YqkR+S5CakS+Rybfi25A5CmqtSpSt1lfWqNRa2CaGrYmkZZOGLbtYG6jRYAkcFtEQYDn+ZhGG/Bl6BAHIZGCeqVGFHh8/nMPs2/vdn78oxe5ct9WJq+6laiyzpc++8/c9OYHSNkJls4eAacfRUTc8gnDED1pIZVPo1gl3z9A5+BGDjz+L+y9+R4KhRK6lMi4iasEsZBcu2MX33jscW7Yu4eiG+KHLpbUaIU+uYFN9CYtnj3wCrffeAO18gpnFgtMDea5dPocA5u3UVgpglchSnWTHRph9eJZSuurZDM2HRkLpQS5XI5Hv/kQjtP2UUYxCCKE0P5XYZCiLdcxbJOImFyPRdqKUMUKhtVG5oaRxNIE1RakncvC6wTYEtIZcH2w7PassdNW/OSc5ME37+GCn2B5tcDkeIIls5dHXl5D92GpBq0YiqUWsSEY6eli89gk8w2Xgy8+Q59XoVJYZKnu4mTy2HYC3dBJ20lagYdjGm2I1VKBmXOLdFkhc03Bjz+yGW9piQf/8jQ/f3039b40mbkFTqzBui+4UEH8nxTD/+NO8f/XNcaK+x5riDemA753fJ2v/uHX2H/HKB8/EfMPnz5Bbucwf/3BW1jWkqR7+3FSaVzfY3xsFLfVIt3RTaarH1NolHPj/POq5AfrvTz0qf+HTmJeOniQ4ydPETSLmELQUALpWFy1o4tUPkndbXDm9FmOnZmnsLZEpdKgNxNgWDrlWNAIJUhBOikZHRRkHQ0pYjKOoDMBfSnIJtrBtSlT0JUUJGxJ2lGEkSC6jB1ICEVaF4z3w1A2plEusHP3dtaqJXy/HYDbBs2LNoRU6kB7fhi5brsYCkkcRv+LvSx1G0QMIiRUAeOU24b8OMJN57lpJIepBSxNL5HvtSi6VT76/g/TOTyEEwUUqw0QGYpVgabrZDM5jl2qUdNMLMMkbvmo2GuzkEUbp6prbXlFGMeoEHQtBh9kHDFbrPHA2+/kbz7xG7ztbVdzzc4+HDuidyzD6+6/gbHJQRzV4vBCzO/fu526F2IZFtLWsIRENywwNXTdwpYKP2rjJfp0i0YQUooCHMtkcbWAIUAJrf17xTF+rGHF7fj/ZqmKF0G1VOHMkbOUGgFhPcD1Axo1n6DRQNcsEukEYaBQ9Qph4CM1hVupEodtsJSUGiEmcRAjFEhN4octdEPDMiT5jMPLz74IUcSXv/0kZnyJEy8f4O2//d8Y2zCFY2n0b9yJDGoYou0OkprWfh4SFr7QMXQNEbq4uQFszSbf3YMfhTQCC4TG6OAYX/nOI3T191OoVIhCHaU5VGstxofHmbs4Te9wL4Gn+OmBQ2ycGqF66IfsnhhiZWmJ7oQkin1W1tewI0GtuIBh2AyPb8ALW4SBolypsef6WwlEhFQmUkmEii83iu0Q4zhS7VGGalOpRODz4x+t07GhAzNpYOu0c0O19iyxv1Nh6ArbaBskqh4EQFMpCnVBI1Kcq8Db3jjMXMPFD31uu34n3bkMPR0OzvgoN739HoyOLGuuhpKC5UunmF+Y4eWXn+Ett9/CLXfez9NLLmfXPHp7RzAsm+7uPNXqOnv2X09zdaGNQujtRSbSlCKdq3Z08eV3d3H28Bpv+vQK/+W6BE9MV3ny8yd59HzMug/H1uN/U0H8txfFy13W+w4p8ek7MiwoQWp+kTuuHEClkzx/dIGDR86xEqQY6+/Ddz3yRsxV+6+nsHgagYA4xrQNogCiTD+nfvIUo4Mb0FZmmJtf4NlXXyZjGoxPDPLkp36Vlx/5Q776d7/Jia/9Dn/0K6/jsYNFto+mOX52las3Zfje8zMkHEmpEVGpRhwvKtZLIWfmYLUe02goXC8mCttFXaMNp2rVFYamkHGILgSOFpNMgi7b29GEJdpEMhe8chlL93DSeQqlCoZptedjUl5OwFHEcYyhtf3GkR+gogDNMLBsGwWYjkMcC5Sw2GJ4yNhlttBOY9n2c/eSlS6ZZIpsb5bply4Smg63338vYRCz7lnQgGv3jVCdPsvwYBbd0sg4ghPHlmkpSaSZeC0P3TKRQlGLIvxmwExljcp6Ab9Ro8sMWJhZxJUmn/rEm8mkfP7hrx7m9//wYQJVwo8VlbUGj3z5aVrVOm9+7z0Ymk0gXXYOZSkU15jcup3C8gKNICSIQmphRH/CpuTW6TA0TlWb7TAO3aIVC3JOmztiRBGaUkhpEgsfy8kQqBBBTLNaZu7UWRqVGrOHjlCvlygsruJ5DfxIw2/VCaOYWBiEZgpH6oReSDKXugw2c4k1jTDw2wCqIMYLA0wkKvbxvRBigWMlCMMQg4hyHXJJm7Xlk8ycOYDSDCqtAnrok8p00UYyBagwIAhcgtglm86xvDzLjskJ/u7j/5OEarF10wgbhga4+up9rMyf5/ArR3jL696I3rmBjZu3o0KXwvoqmUTMpYuzJJw01165iWMvvci//O3HObnmIeKQpdIqhteOz7OtLN1DPRx99oeUqms0GiUcO4uMQvoGe3n8q9+iN5FFqQCEas88hY5Aa2PfVTugAdEOW1UCgsDl7NkmzQ0bkK327cuI28+6bTl0d1goTISUJFMGi3OSetXC0qFQ11kuSP72u3McvrDCQF8njpXkB4eLvGlTDw/91g28dWeCv3j/zXztb+/nvju3EwUBTzz3EzqI0AtnuWJrHycvzkG2ox2LpmuoOCKTSPOD73+Nv/vLT9KqrBEEIX0DI0Smw7HjJX7liwU+8FiB/3FbkkemPTrrAVcOSKQpOFX510ibf1tR/Dddn//1hLHkUyeiD3/p9ckP/eNhjwHb49SFGm65yT+/HDI4PEjV9SmXSvzGu97OQ498g97xnbSaNQSQ6+gim7KYm7nAw5/9ONa5n1BdW+OzP3iBhOmzuNpg40g3X3/yOB/8m0f4xDdfJAgifu62XXSmdF44vsq2EYfnXzrMSF+CroSLXzfwNUl/QhB3duEiqbshmCatsP2A1DwoulAO2ob3ZqBw0Wm5MQ1fUKspKi6UWpKapwgjkLqkEMcMToyRyQ9w4uhxpC5RscS0dFKm2VaL0H44DctCCkkUeIhYEYSXzdRRSMbQSYUtMt4KTU8gIoW561ru2TdC4+wZDMdi8y0389d//S9sGB1iZXaRYsOlS7RYaNT5yz94Pd96fIGFs6eptEJc3yfGIFCgohipK7yqx3VXT3HrzTvYccN1/Mov3cb1t+1haGKKUEuSS6W48759LJ1fZ6msY+stOnuG+Oo/P83YUD9LS2v0DA1RXipx8sAZhke6eeZEiffcPMrzp2polkmtUCLSdXTZTtqO/BAR+wRhTMI2sKMAT8UYQkAkScsQX7blQHWl4UgNParSbdqsuSG+HxIDBgatKEJFEc1mTIxAEpLt7UYGAXHoY2o6pWIdREzo1anVGkQRCCS5zjS6ZVBcL2MIHUMXSKFj6AZCtf+4PLeB0DNce8+9qFKBVuAxd/wC0jJpBWC4TYxkmpbv4bs+TjpF0tFZWirT193JyZeepaIsJno7ePrwKeamT1FvljjxykEOnF+gN2Hz3MGDHD9ygIUzR+nqGWLj8AZ6e7p54acvMLRhiqRtMn12ms179/FrD76JchQzfXKabdu3Uml6+K6LkCYvP/cUt918F2dmL7Jl4wghIT0Dk8xOn+eJ7z+OYZioOEAqCVIhidsoUu3y6EZdHmMLgS4UKyWwczaVWsD6gounm5SrMZGKaLbaBohyQ7GyEBH39VKcreBHgkI9QumCW7eneHW9TLW0xmqpyY1Oi5tGbA4VAl730Wf5vS8d5XM/OouZMgjKTU6v1pjq7+amiV625DP0btnFTw+foDefb4+h6g00XaMr38+JY8+xd8tmjpw8geGk8ZXg/FqL+UpE3pA8ddEnbQtUpFj14eA6/+YO8d+1KIIijGLWPD50x4DkuekAPWXQn02xqHXjJBIsLC3wwkPf5D1/9AFu27uH2aV1lGZj2Qa2JhnozjE2PMIDGzTU2Vf58k/P8OjLB+hydNaqMa1yicGky/Y+QU4PeOj5GVaaip0jBvMz5zhwbJpq3acrJTh+sclzSwFRDCv1iNvu2k6cTjA3vUIpEqwE4ChFWUmKvqJ1ucNbjdpe0LWWIpYK39FxI4FpSkqewg0lK65CVzqpqe1sH0/y4xdOtLtNFRHFbUKaEO0O2LIdVCgQGigVEasIW9MJo4hQCDq8Bjl/hUokSFsCT3fYvHcnW3cP08pvIH7xeTKD/UT9fbz6zCFCYWEFLZ6/sMRjn30P3/3CQ+gzswxv38LgWBc9Q1kuXlhri7Jl+4UIo4CjZ5fIdtm8+N2XeOzpS3iezpb+PEYqxa137ySolFlR3Zx78ieUfBOlJHrUotgS9Pd2UCqUKVUKoJs0ay6elWQ0Z+DqOo1iiVA3kEJgWgmarsvWDovFmouhNPxWDQ8dRxdEUUxCxqwYGeJEFk+ziAIPJ2mSjAO0WEePfaphhIrbL7ZQBn7gEeGhogihAjSloYQgaEFoGLTKa8RBmyhYq1QRKOI4olhr8Lb3voPxiTEKK0XqjTqBFxEEPiqWGF1pXvfg+1gpB2Tz3WzuybHajClUl/jlX34v337kmwxnbRqxTbNawfNaWJZOOpNmdmaOvt5hrE6H3fluXpqd4dfuu5t6qDh46Djlhs/2LRvZsnkT4yM9bJicwDd1Xj16mPNLZ3j1zEVwYwa27MCOfM4vrHDHvis5u3oJM05w7uxxdl1xJWvlFqHbbMtrlMfgpi3MTZ9gYKCf0G8RS4sbbrueWmOVIy+/gmWI9kaQtkc5upxrIEUbSqUuN1JKCoKmS9Jw2HJXP0sXG7TqLtiKekOxVFHUQvCFwrQ02NLFxTNlclYbUVxugaPHVBNJXni1wgRV9hl1Pn+6yFs/dYpWw6UvKelLxqysV1mohkihcXZphes39DEVFpjctJO5iketXsUwk7TcFp7bJJeyKK0s0NHRwxV7dnPq1GkSjo2lw26nxsYewQ1jFq+uK9601eJzp0Ih/p0K4r9jUWwLvM8Vwg+vCftDPWaIDBVn9W6EbjE9P8cH3vlL/PHH/o7+ngESlqQW68RBSCLdgWlZnD99jC996k9wf/RVlgs+H/z2MxQqNVJWOwfxwdf0sX1DjvnVMtW6YqobvvXcDEYigawucuKSz+xqyHrJJd3Vy+7t4wxk2kuQt77hCj7zzRcwOru49YZJ+noT/HC6yHRTsRLCgq9Y82HdV8y6imYIZzyYbsSc9xRnmjHzAZz3FXMBnC7VKM7MMT65m937d/DSswewbZsgDjEsE0PTEUIgaW+d/TBAXo6rdD0fXUGMYrZUpVTyKbTgbE2xLC1+/u03EGZ6UGeOc8dff4j500e55ro9NLw6333qDG94w7V89S+Ae2jaAAAgAElEQVTeyLNf/go6Pbzt23+Ocekwl5ZKaF6aZG+CM9MlwsiH2CPwA3RT5/TZZbbs20m0eJLi/BwHXjnJzPnzQBfffeIQ0eRexsfylOZm8Vse6GlOHT1AfmCEVnGdtYaHjHx0QuJWg3K9yq27xnj0yCw6OrZlYhrtZUnK0Kh6LoKArqRDHLehRRqShmZgdHYTeg0iFUEACVsH10eoiISKWfEjHNHuaGxTEEUhsaYhwwjdsPBbLramoarLFGsBIoyIVEyt0cDzfWIBUtPx/IDOfDeuMNj3hl/kjQ++nanRBFfe+1b0aJY3/tqf8Pm/+O/ceMttJFM+qdwQZ8+dYHTT1fzn9/8Or7/vPs6cmEe3NcrFVcI4RrcNTN3g1PQsN95yB3PTr6CkwamZBfTY4Fs/eZbf/LX38MCbH2Bx+jSNKML3Naqry2weH+I1d7+eaq3F6soa99y8E5O23bRPhDx55AR37L8eJXWOnDzH3j1XUqrWqDRagMKtrDA0NEIz8LAdk9ToVhKmTaFcZuOIwdNffpQOJwumTtNVKBWSFiGKGF21SXle0J6TK9qz1/W1FtEy9Ozo5Z7f2sqxizHPn63SM5qia6QfP5nmfDXGSlt0TA0zc2mV7J7N3LAlxUdfLNLr+cw0BZuDEGN8nL85UGPECXAAy4COhOJ1+3QKFahHGouVFps7U9w40QvK5c733s/Tzx7HjdqAusj3CMOQzYO9vHTyFHunRlmYX6HSrNGMTR7YrrG+7vL4dMRNowZ/80ogYhXz73kEP6OT78geHB3s3Xt+vcp77rqNu/aN8MOXznK05GHaWQquT65zEL+xQrXa4t0/fz9vHV6n9crLfPKlef7oOy/QnbYI6y57N5rs25yj3BA8+vwK/+2de3j+8CpjPS1+eqLGu991P088/A2iWBFlN/GffvVuvv7dI5y6VOCGXWOgYHuPolapMDQywOJ6DTPZwexyhYRjEoWCfIdBFAvqzSb1lkd/vpu6B9lMjshv4AURyYRNsebTZYVUqmWsZAq30eDAWY+HHnsBw04SKUU+ZWAYDlLTMImJpUCpCE3TabbqGMJgqD/HA3duYrHapMOxyGeT+H6DbHcnl2bWkVFAJuvQN9RLY6VIV38Xa8tr9HQkadSbNGoe3cNDRNU1Li7VCIVB0KzTP9BLqVgllTRptFropo2pS+IITl4s8NjRMtraDAkFc26M1wgw00mCZovxrXvwZ46CY1OKNBr1FoPj49RWVii1WujE4PtoWkxNS/Hr+/L8y6EiGwYznJupkkoaEPvkEyaLqyViqbCEIlZaW6MYx0SGQZDNg9ug2fKxDJOkLUk0WyAkhXqJSqwjVXtz35HLsbxSAF0iidBkAhX5/MG7rqDT9Di70NYbHnr1BAlbZ8+WjbRaTVCSC8s1xka7iIRNqjNDomOIzROjVJamOVeAaO44O66+maW1dYLiDB1JnZmSIJXJoJsJqoU5sppLNdBJ2+18ynQyQYTiS987juiY5KarbqVuxXzj7/+e+WqFH/7w+/zJH3+I3ddcxejAMPXKGrFU2KbD0so6J198hi1X7OfQ4YMcefEEdz/wejrtFF94+BukYo1fet87qRUqPPHEY7zjXb/KmUtz+G6ApilM3WJo0xhnjh9j4or9OCmH4ZEpbEuwcWiVpz/2MZxUmjiGdCZFsRET+S7FUpFM0uLVsysYpsZYX450OsnMcoGEcNHMNLVGjGZezhRNZWi5PrFfIenYuKGGH8B8RTHQ28H3nz3Jf71vnLlKgw996ij9Vsx/3pniYi7PM4cv0WjBvdf38NOTdc6vNnntlQnq9ZAfHPdpxIJ8yuHJ993FxFgeuWsXv/2lw1xaqSGtJNJM4q3PYiTStGqr3DSep7+/l0888hTLhRLdCZMjp8+JKGwvk34WR/wsvlABt+2cVOcbEbvGB+mjzKX1GhuG8lgCFu1BCkGSWOpoKkCGId95352ceeY7yGKRnZ99nsG0xm07Uli5YV586SR37M0SyCRzyyXuv7aLdNcIH/zYYX73F4f4+lNr7JuAo2eLLJdMxjsCpt0sW4bS7L9mB5/86gtEoaK7O8VqyaXpR3RYOuVQ4bVchlIxWzf2UfclZ2YLrFVamJfFxQEGo/kMSilcP0T6FZx0J1HURIt8ursydPZMIO1uDr10DD8KMSRYtoFlOkgilDSIwzYPQsQxKgrRpUcqG+JFGuPDA5RLJU5cLJKxFR3pBCU3RotjUNCbT1KvuximTrGlSGuKQGhYpoWmwUA+Q19XF0dPTCMIOD1bIekYpNIObhBRqHnkkglqVZeslUZaNm6jStrScVs+JbcdkpuwEigV0qU8VgOF7wd0Z7L4cUwzaJGVGk7WYXzbGH61yty5VV57/Xa+8sxxbn/N1Tz9o2PYqk42kaBcL6JpCVToooTEBlyliMMYOTKBVynj+QGpjInV8rCDAJTgfL2OUgpLQf/mEebOLYPRjpVfbsW4MWwe7iTXCd3D/VQLJWrrJaRuEAYRi40YUxM0/QgQWKaOcus0AkHgesQqRtN1VOgRo+FYBqES5BwNISEUGrVKnXog2NCXpun5eLEBmqReqZLPOWgqZHbNpSvTSRR5GFHI6WKTz3/un3jqicd505t/kScffoiLF87hJLvRpSLb3UGmM0E61cuRgy+wZeMoDz/+A6QX0sTkthuu4dTxQyRSPVy3bz+f/fqXef/v/hanz61QKRaI4oC+yc2UygvExSXs8a04mRRjg8N0ZPPsvjri3Tc9iKkHXFhqkU05oGIavkRYDilTo+xGuG6Lar3FQD7NSiVg11gKR1OIyOPwrA+mha0bCE3Qcj1SifYGvxnB9iGHwU6LHx5aYaoT/EqLsxXBzf0awxv6KCwv00gkKHuCfSM1ymWdYysa10zGPHsqphKaXDNu8p2XS7x59xgfvXULJ2shEz/3Dvb86p9x8w3XUyiXyCYsku48HVGd0ytlao0mm0fHef7iGnEcIGs1Ts6vCAH8LOqi9rOotHtG+lRTSoQmubErZq0WEGiScsNjptigI27w1hv205eD84dP8+1Pvx9e+iGZ2irPr7n86Pwir7lqjIm8oN9cZHrdxgt0upM6qVyOa3ZOIvwaW6Z6OXS6hIx83CBG1TxOLEdc8hyySZsrrtxEV0eWu68eZdumPgIv4hdu38rNe3qwdMEH33sbuzak2TA2yGq5QUcuyT03bmcwE7NjywQ37t/MxqEOto8mWVuvkLAkN+0boV6rsX/fXgYnp9g2OYSlSeYWq7RaDUSs8OOo7TsOI2ytHfwaK4kSMTLyEUaK194+xYapCTaNDlCYOc/FS8uMdwt2DOssVTXuv+NKhgbzbJsaZPHCHGndI/B9JvI6zdDitlv30t+fZ8/ujRw/fp65+WXqLZ9ypczQ8ABX75wgMtNM9qaorheY7DcY6VI4BnhGH2GjRRAFOJJ2hx0DgY+QimbQxrmlbB0/8PECDysyGNzcy8DICE8/c4RKIaQWaXSqGvXY5MypBa65bRuaMFk4dRYjlSDyG6hYx5AaQgjiUOGqCC3RAZrCbzRJphOY1RaWaVJ0PbxIoamYkclxZqcX8SzYnDZImYKKr9rhwjmLHUNJHntumkEnwpIxgdtktdDiwVtGcZwEG3uTpBIm3cEq59YCNudimq2Q19+8ib0TneyY6KEzZVH2QBLRiE360xbjG8fozibpyiZYXqvxhq2CSl0xnJFM9DmMDw/SncuwYzTDcsXj2uvv5vipw/ztJz7D449/hz/8wB/z93/2p+i6iWmnSGY0LNMBBYaRoLC6wvjEGM+9+ApvfcMtvHDkPLqEmlRMnzxJtRVx1aYRpk+fYf81NzGzsAKhR7PuMtDTy/riAZzEOKVWga5UN6lcFj+M2XrFLqrHnmFk2w6u3TqIFyp2dDdp+bB10MZrtviFmyYwDMFVU9006h5/+NbtrFQickmNpuzgdVf1s3U4zb3XDrM0v8R7Xr8DKS1u3TtMfyqmvzPJcrFGNu0wW4k5teqxzYF9wyYvTZe4YU+e751u8sa9JnUvIJlKcvKSi9B1Xjgf8bbrHLJmnZ5cgu+fKfPuXcMMGSHJ4R5u2rab7/z0KL9841Z22A0ePXiUWt1DComKNaRfZbwjyfOLZXb0ZihVm0utIHrl/4pOUZeSu3eNqTO1kP965RAL6yWerytqfkhSQNFVCGng+zVSmsnn/+dvMjmUZ+mzn6QjbTHyseex9Jg/fsf1vPbqCY6+epTf+/gBPvL+e/ne89OcnV7je1/+Db71+DFqa7OcmS4yNWDgxZJnvn8KXYf+Dsl0mCchGxQ9m2Tc4nwxoNtWmCKm7sZ4uo4W6QhD0ZExMGWDhm/SlU3jhy3QM9imQUfOZH2tQEfSI9T6SetVzs5V6Ug75JyQly8JBBahnsKtNkBqqLi96LAvd4wq1gjjCF1rFwbLilFuHVMqkrbg0mqNjpQi50gMAxZqGpBGRAEjPRpHLlbYOqTQ9csZj5GJH6ZI2hJNxZS9JqN5AWHApbWQ2O6jw9awbEXoehy9WGAoBz2dMLsMTjpPIzVJY3YawhahkHghICKkalsRBaBJQRi1rZG2Y2NaJktrFdJ2+zoOMb0dHUx0ORw8t4a0FD2DAwyPD3Pm8AkaK6s4KQsRt0cIWqzh+S2c8S249Wrb+55IEFSLKD9iqeGCjOnq6GKhWCSRsNnS6WDhYivJ0ws17t7dj6Eiap7HYiVCRjXu2p7m6KUqHUmN712EXilpRqB0DeXWmeyG80Vo+GDYNq04xgR8pZCx4vZdQ3z34Cw9OZ3BfIalWkSXIzh1sch4ru2dX66DKy1Guy1CV1FCx69UGO1Oci7s4/p9e7j3Tffx+b/4CPvvuofGeoXG+hpKXf4NhSKZdiCWxLHP+PgUz73yIhu7Enzyq9/nxiv3cvt1+/irf/oCn/rVt/KBrz7Mr/zG7/Pic8/j+i6REmwYGee5Q0+y55rbiKSEsEZ2aJL9O69lYNziI7/9LtZOzWOGIZGIcMImMp9m0oo5t+axElhMDuU4dGENW4eyr+hKJFhZr5BOmAgE5SAip0nmaj4pXQPZXjQKYjQEui0J/Ji+tMAvBJQ9xd4Rk5mmYOOww9BYJ4/8dJWPvGMrLx5d4NGXVnnnXWN88ZlFrttgcfv+DZy4VOTLz1xgX3c/X3jdJhqeJPnX/8iXf+cD/Pm3n8bIZdGURJfgWFAMFZ1SZyoRY2U6eeTULENC8cz04s9k/Kf/e3/hRHdazTR8XjOU5WKpgi7h7hS4ERxsSGwnptxyyaUSRGHI1z7zGX7jxk10JQV/9uwF1mstHEPwmYd/Sq9WZv/uTcz4h/nWQ0/yiY+8g9/7zFEmrvpjJneM8+nfuoJXj14kECMMjDtM9dmM9rjUGjFdw91E1ji7hwy6cxZ1D5ZWW3R3pymWmpQDnY3dJkvlCN+ts2GsiyAwKNQbRFFE4DZYrsSMdTsY1hhRGOBHEqE8RsdDLEPScENGJhSVissXnlwlkzJoNvz2lSyKCZBIXyG0CE1KolgSy4iJvM077hlDailMA1pNj5nlIsWaYnJAZ60qqLoemUSCZqvFHdeMY5mwuN5kIJ9ACAOpacyvNkg4OlIFdKYMTl4s090nCWMY7jKo+DaOpfHA3Tuw7bYUJQgUxWpMnOjl779SoLEWIEWEjkCTGsFluRJCEtJGWcYoEobBUqlKIuEQBAFKaNhSsVQp0+XoaJYgDiJWLs5RWSswuWUKb7iXi6fOIWNIWA4qDEEzCEKFZkjiUBIEbcj3ctNtM8U1k8VCicGOJP0ZB82vE0tBJmXz4H0buXvvMGfPLZBK2vQaLTb0ZTCsNLcUCqg45gFMWpUigR/g1taxjDxKamixSzVySDsWli5Zr7ssVWMGczrH5mr8wb0jxEDB1+hLaZRbIXdvzTCe8yjWIpq+T8lPsFD26EpqtDyflXqeRsvl7Xu6iXIRYWWFjZu3UFmv0dvTTRSG+M0autDRpY6eTFBZLqGbcP70q2ScLJ15ia87XLd/B6dOHGd8sJPD80tYUkcKA92QhM0mURDTaLp0dnRhxCF62kZrWuQ78gRRg1pd55qpNE5asFKV5LMZdNlFFGsUm5Lr+n0GB/O4oWK9UuH2Kyeotnz6OxMoIam0fHQpiUJFFAaU6y5pO0ZKkzDycL0WPR02l4o6K6UqzUIFkYWLixCnYjRP48yyx2BPiet39vGuPz9INmXzO28YJ3JLnF9uMdZlsl6scOT0EmdWYbWywgefkrx75wDmD/6Kb3/tu/QOD7VZ1rGHZVv0G4LXZyUzQUxFCfxqheGEiRZHJAydVnhZlP4fuVO8Yapf7etLM+AYrLg+Zqx4pe6SNxOkdWhEUPU8+m2dJT/i924dY8J0yLYKZP7uFYRQXDVicWbRpdMx+Idf28avfPIkKRuuHEnyvrt6eGXJ4opBjUK1xacfPcP1Ozp4/pUCQ2lIJySHLsRY+RTfPRczmgVdC4kiQYqQOIoIQolpaDSFQbPVpBlpGFEElklCgzgMSaU06q0IPxZoYYSrIJVMoHyPnpRGEMXsGREslGJqLlyq9xPFHkJEJC2DWlOQ1GPSjt2WiEQxWDoiVES6REUFDFOiKUGAxLFNiAPCGJK6ROcyZCuKqbshaccgjCBlKGzbotIM0TRBw/Px0GgGIbaukbZMwhiKLRfP9zBNCz8IcBwbwgjpe6QcHdtOEMk0yxfnkbaD64dtKlwUEat2SrgfxagYTFMnVJIwCJAyBt1GRQGOrhPFiqGMjTQdzq+sYZkaCUJC0Q6UndgySWHhEsWlEknDwCUmTnZgpNIot47h+6w2Q6QKiSKfDjtBT9aESCBotuezUjE0OcBqcY1Xz62jd3ZiiYDVSkBXJkkQx4QtF1CUvBjL0EnbBlW/LQHSVUALHVtCRvMp+5KpDouaF9L0I6Qu20nrKkYTIHUdooBGqDA1AynbSfVBFCJp0xstHQxNQ9NE+zXyXcauuIW919/OpWNH6JscQ4YhWhjhhz6x20SzUsxfWiCSgqStk+/o4OLaMjJukZYZ/EqBczMLbOw2mQ7z3HnnbZw4fIZaZQ4Cl+7+ceYXTrJ9z7WcnTnP8IZJOnJphrZfy+DgIN/4q/dz6XtPsRKDYWksuRHS1mh64AURPY6gFSnKvqA3AW4k21bLqJ2K7rkh2ZSBiUCPfEJ1GQeSsDB1CVKyVArI2IIx3afYgJt7QDMUet8gR1ebvPnGHjAkZroPFTQpLC/zpw/N05vRmOozGeo0+dhTRW6fMFks+ZwswqG372DP9Tv4wkMv85OyIiUVy15M2jLpMjWO11wsLWJAs6kpxZitc6zU5MJamSOLFfEffqb4R9dt+JAPeF5AXijqUYQhdBa9CC0K0IUgEoq5Up2cpXHzZI6hKOSXH53m0FqdfEpn/waHt9+Q52JR8qPnLmEQ0tvVyZqV55s/mmFrX5Krt/QwtW0zpy+WGZ7s4Is/WmdLj4aVMNkyoCibA0xsGGLDWC/z6x5d3Z1EQYuB4SH03lECYdLd20fGCNi0cYy8E7Jv51YM3WDPsINlm2weHmJwsJtER46hZMSmTVN0JRwmN47jhZL1is6N+ybxtCyVeoTXCkg4Jjfum+KKyQ4wFIX1GkLXmBoSlGsNmp4GccDOLRP0Zw1GBrNsGu7iqn4D6XSiuT5dXT3Ulc5oTxYnaaMjueLKTbjKwrUyGKkELWGjWyZO0iBpxNyyfyOLLQ1VLNPRmcTIdjLQ30N/V5qJkR5u3dbNQjFkYkMfhZKHncmyIVOn5ZksVZtIpdqWOGEQysvJy1Lgx4qkrdP0QnRdousWYeQTC4GKYmzboNYK6Erq1D0fX0VEcVuXqQgpFipoVor+0TzVSgUzUqz7MY5pkpASr1Ygr0GHadCVTiCiGCE0jDhAtxwUJlKLKdTX2ZZX1N2Y9949RS0wkabDyGgfATqdCR0naTHQkWHLeBd2Zyf9PWm6Ox0SKZPdQ1lUOkWGgNHeLJ5pk0hnsAyT/GAnG/tyRMKk22mPDzQnCTEMjw2R70xw4+Y8mukwMDqAaRp0ZhNUaiHZzk468530GTGlapEwguGJSUQYMDg8Sravl2LFZ8st9zA5PML0qVdwUjmCMMatlwjtNLv6QmYKCq9WpKO3gwPHZ5naNgmepFBaQtfBtCVepUk5iMh3ddFqNRjdfgX53i4cGdAMHJJWkUeefQWVNNiytZ9cR4J0yqDfVPT1drB7UwdJDEIp2dWXIZFNMdafZSDrsGE4h9fyGBsbJJtLodsWffk0frNBZ1f3ZeCZwZUb+0gmDLJeHV0XEMFMCbZs6uHoUpMHd6fBkJw7N8sjzy7ylZfL3DCV5dxCg2bN52Jd46YRjbs2myg7xfmVFr+0McHA9j6Kxxc51vQ5Wfa4qddmqRqy6nkkTZMBUydJQEbTKHkBI50JklJwZLn64f/wRfHQYu1DvzCe4a6xLJ6v2shIFbHRgTfkbd7cY3K82mJrNslgUjIh4OBsmQ8fmMeQMNVtcOcV/aiwQk82SS4pcByTc5dK5GkyONTJN567yFd/dJFXDp/E8wXXbDZ5/mCZu6/N0593sBzJ4qUyN1yR5bmHjvK2B7ZTXV1jz8Y+Dhya4Z2vnWLPoOSGXRkunZzl3Mkl7rxllNbiJTqyFtdet4VrNya5dmuSi3MVGvWIzX026cYCo5mQK/ePcc1mhzt22hw9dJGt/Yr5MqzXAjwUd+5OMpgRnLlYYLWqMDWBHrmMJ1okkgbFps7qapndmzsonTjJ5FCWF48VOHB8hj945052DgruvKKDHrWEXp5l+1iKtaU1nnvhHA++ppecFrB/CFJ+ge8/s8A9V+V56tlpEo0Cv/ngXobyOlM5hV2e4bGnZrhueweXTl6gJx1z3Y4urp1KMNUnWb4wT1Cv0sKiqQwiFLqhkZASoWm4QYBtGASxwJAasRDEKkSTOrlMB5FXh1gRKtFO27ZtvFYTITQi2h5vzw9wXR8hbJIdefSECV6AFBq2Y+DWPTTTROiSwPOQmoYlFC2hYylJZ1axcc8wp+Y93njnJn58rEilWueNtwyRd6BVKqJXFjg1Xebndmk88vwqVw7HXNEbkxYKM/Z4+uASS0sVBiyPi/MNNg9oXLttgMGsYqrP4F++d4Ehq8T0XJVS0eXGXX1cP5Vhz4jDcEbjuRemmRzv4/Fnp1m8sMI1GzNkohYbey3SqsUXnlvgzp2dHDlb4NBLJ9jT1yI3to2fHDjIX/73j/LUs8/y8Je/SENavOae+yguXsAPQrxWwOSmUU6evUQXirKn8OotXjxxhntuuZNStQ6Bi7QMwmYdU+qU4hzDg92srq+S6+khn83gNddpNlqIRCe5C9/jxp15slqDVq1JJm6yXgrYONbJc68W+E+vSbE6X+KqTRZXT9r84/fneese+NyP12m1Iu7ZmWHYKrF9yGal2OCleZ/7d5jMF3y6UwZrc/Nszgg29SqOLoTcdbVOdSVGS0tqXszCapl/emKJx16tk0saDOeSzCwWEEJjuEOyq0fjumGfrXmbRw9V6E0JXp4PeUtS4nkBnpBsSBhMaAH/ZXuOdBDjiJAwUFiawW29SUY6Dc4u1PjHQ7MCIf7jL1r+VZTTlzLVn17RzzXbephdczle9Kj5Hsk4RheSJjFaQucN+ST/49UiXzq+iFLwOzfluHLrOMMdMWvra6wVCoz0d7PWSvHqrMv8coWuVESt6tKzZSe9ep3X3mTyhS+eZ/MGm/NLPmEccetrNvPEY+dIdSXozmc5c2aJrx+s856rE1xca2fxFUpNTAFeJPmFX7wGf/plZtw+jry6Qld3hs6uBAkd+kd6SEuXnb0VTq/Y/PTVAoZhUAgEW3I+V46ZPPKq4OnzGpVmi209kvFeCz+MeOFCTMYQZGzFPdtNluoxZ4oWS+sRQRSwa9hha09A0oLZxTpn112k20ST4AubzrTE9WOSto5u2cys+QgREvk+fiiY6lPUQoe0GVNsSqrVGpadoNl0Geqx6OlIU675uEKnWmsikNTdgJ60RtJpM2lagWK6aFFpxeia0RYKhyEJ06QVRCC0y6ELOkKApkkcKZGGQOkmbrVE0kmRljFzdbcd+KDi9lVbaERRGz3rmCbpbA5b1yivrNOZTdMKvHYosPJJ/L/kvWeQXNW5/vtba8fOPTlrNBploYAAgUhCZBMMiGhMNNEYzDE2BhOMMQ7ghDHBYKIDwmAyGCQbk4MAESUkoaxJGk3qns690/p/2GPu/XBv1f1yDr51dtXUVE3o3r3Xep/1xufRBH4gcESoumdRwtdzLN5zFictaeKXd71Oa3stfrnI1kGPTSM+5x7dQu9AlfHhEarSoiEWaoWXlWDdjjKJuEZjykIzLCwCdOkxXhUUHR/b1BDCxNRcyuUqAkXENsgUFblyqAOaMBSWqYEPtQmNbEUwlClREzcpVx10qdFWFyVbqCAEVNyAnoxLwgIrYpKM2NRakkzBIV+okOrek+tv/ikrlj9AfUMd8ajks48305K2GBgaI+oUeeDV1fzs6h+wYdMOnHwGTfepFLNI5WG2ziURlHFsQW1NI/V1cZK1aYSZoH5yC7894wRS9Wm27yyQiARcvJuiuT3Bs+sErqbY0O/SWBfhrx+McdgUyeCwYunsKO8O+WT7q4y7immTLJJVB0eHqmnwyVaHRy5u5c1NOTasL3HkXkkGM2U+3OYwp0XQbsG2gkVNcxo9Eee2JzYxvzvCaNEjYukE+Bw0WUOVKmiaoC4Knh5jXV+F7UWft/sUH548hy6typ+Gfby8y8KkyepClfqISVXZdDZYTDLhyrd6eGzj6H9bf/V/a/P2vy9LEyyoi6pbj+pmn2l1bPlkJ6t6i2zQNb6rF9m+2xQW3v5umH+aKMhYUYsHL92TPWY3snWowuD2z9m0tRdDh/pUjNraNKt2xDnvK0088FY/VErUu+PohiBT9Ng55lHMuXNYwHgAACAASURBVFx6chOPvRWlMr6Fxz6APadHWDdQ5uR5ERbNTaJ8nTU9LjllESv1MXtONz1bt7PH4oX88q9bsaVD/64CuibQo1ECO8LwuEN+vAiBT33KxjA1dN3AllG2jCjG82VSSY/ptYpcVbBmQNIU15FAjZUjloxTqZQYytmYRpQoFcxkgs+376A+Ab4vSMZ0dN2gXK4SEOZ1atNx7EgCJ6gifEEsGgKW7ymcapmdY0WUAB2fkiNx/JCd3PEUbU0pCvkqGgG6NKh6BoY0qGlIks8O4wQ+rhuQLQSUKwrNNCbGs33Ax/NNlCjjB4QKhFoSO2lSG42wa3iIwPeQQUBzOkmx4pAtV8Lma01h6EaoMet6VJ0AaULMjCGEpJDdxaG7z+CzDdsQdgKpBIYWUC27HH90J6cvm8uyy5/nB8fX8PiKLfSWdHIu9GV9pnekmdtuYbl5Vq4tsTUPNaZi2qRaanSXtf1FdhYEMVORtCRoBn7VoSER4ClJ1hHYukTqOq7ro0kN29SQuh5KpxqSfK5AvqSo+sHE2JyGbfiUHBAqwA8USoS0XHET0okIkYhFxQswNcnYeBlN0xjLlUEE5EoKXJ9zzr2UyR0Rcpk8ejTC9t5tpAMgP0bfzhHW7Mxy8XkXsH7DZgK/SKWYQ6lqOBCQaqOYH8WM+9QnmrA1hzn7LWYsU2DmXgdyyVFL6c35dNVaNNdEiZiKfLZEpuiiPHBEQH1EQ2o+J3cK/jGgmJFWfDYmmFJrsG+nxQPvVrnhsIDvveAyvcXgk36PBgmL6iOcdWyMVz8Y5R+bYVpjQKcOdkxy3weK+24+mCtufpOLj5vBYN92cr7DeKZKXEBnjYWtexy63zxymXF+unKU+9/PkpaCBXWw3Yvx+eWz2LxqJx+5Bl0ll0UHt1K0TZ5ePcz3VmxluOIKX/13dSf+D4Li//0jdKZtddS8ek6d0kDcdVmAy3Grc7ywaQipSZoswXFTFJ5h8Pf1FfaYHOXXF+6LTYUyOjEryo6+HdQkolgdnXzt5+9w3bcX8dDNz3L5GVNYt3mUDzeXWP5RGQ+D+TUO93x3DpfdP8CsWpcXN3n0jTl0pQV7NAm25jQGypJcycWQgquPb+fBl4eI2rB1xCdXdjEneP8IAoLAByERQjIhdIYQoFDETYO6WDPgU60WyDgVqp5LRDdojsRJRzR2FioMFvOE6rwKXYPGdJKiE5AvFVGE7OBSCjRNw/W8iSc4MesufJQnQm6jIFQrFJoOCLQJ7RcQIBRS0yfoonyCQJE2beKWTkKT2LqO4ysM0yAZizKWK1ByqqFyG+FraJoe6stIGRq/CiYKMAGmGaemvgGvWqaYzyD8AIQiadvoUjBcKCGUj2booHykZuC71VB6wfOoahqW0LA0jURQYkZtjA8Gy9REIxPC7BkWT4MPtjqcd8JUrvjzdsbKhLIFCDRdonyfmK2TLwdILSQ4CAh/Lybm8TUhqNcFhiYoeSGhsKvC8TZNKALFxHMKEAT4Sn2xNmqCpFXX9Yk1ntAsmVgfNcE6w8Q3X4WKk4pgYsMLpAZB4IesNQqEDBUdD1i4J+eecxrjYyNotk1fzzYqQyOkqPDWp5swUw3sv/cB7Brox1dFyoUcSjlEomkKvo0XOEQSNomIxDYEU+fMRI810jBpGleecy5OsUrRdcg7HkoEaEKGNG0SfD9ACUWrHbCzLNinRdGXV2zMKOIGpGzB1KiPHRGs2BbqEp2wm0UhF3DmkhQfrhvmzg/CusDMtMbRjQEnTINgUpyH1qdp62xgrGc7PzpmCmvX7UAJg/qIhhGJELMUz74/yvUrx5jbIkiYYCP414jNeMFn5Qnt7B8z6I3avFb1WL5qgFUbMyLnBv8viPL/U1D8f4ZIQW0sduG5u7fc8+u3tod8f4GHEJJoNM45Cy1m1fj84Y0xOmd2s/8kk68cOJ2mujiNba28veoz9rvyBS766mw+fGs7V39jKite/ZysnuaZ90fwlUAK8JRGq+my91SDpzdo2IZEqVCvJACCIEAP/AlWbKhUfExrIkQUofGoCbYbKSWalCilEUzoHmt6aCRChMbYGklS8ELwtA1FplzGUwrbTBDHoaIUedcJ+f1kaH5CgOd5KBmKk4NACklAODAsNYnyQ49E03SUF3zBfi6kwND1iXEnie97KCG/CHGDQKGCAN0w8HwfgUATECiF0CQW0CwD0AyqKjTYqueE6oJCQyiBFwR4KgA0pC5QQiAVaFIjUC5SSnxfYRoaBhqpiMFIPg8yLMRIGRJkeIGPK3S8QLFng2Kk4pPSAipGgv27I0Qa61n12gawbCL+MNscm7gJ24uC3lEXQzfQTfMLpheUCLkpfQ9N15EyPKiUAlOGLEVCfaFvjhOETeqBYIIgISwp69KY4LgMJl5bhOAWhFT8Sqkv6LekFiovKjWxHlJHBcHE4cXEZw33ldQkBAI3COnLhJSEI7rhrrrpR9fjV/KUykWiOmxZt50O2+ehl97iqEOPwrbSlAtDVCo5iuNj6PEowtMhkqRSKpFqbSRmFIgnm+iaNp3azm50Xedn//VdBvqHGAm88NAUEt93QYUALRDomoanFEIIyhUHIUEX4UEQBApfhWsmVYAiIAgU0xtMfNdjS1YhxL9H7ATHzBScVK8Yj8ToHXTYIpM89ckIlx89jd8ua6GSK/Px5iHW95Z4YlOVDzbmOHWWzuKZNt98pkjGDbspGpJJfN/lwqkJfvRu7/8wLv039yn+fxEzqEkmD03UpO95+PMstakEY+OZCXZHgaY87ns3IGVWuHivCEYwwO+frXD74+vZbUYD1938NU67fgXTo7BtXS8XnLM7TbFxymaMv701jKaFQt6aaWBJjYJu8UqvQWNaCw1HajjVMrpuovAmJALAEDoiGRqAQqJJDcM0cV0XqUm80Ekk8EPmaiaMRUxsPIKAcRHg4SCkIF/yaIpF2VGs4AdVcoRAa1sWSoWvr+sKIfUvvAldN8KTHBBCm/BIA7xAwQRxKKYCGYJSqHjtI//N7C3CAycEQIWSEt91kRJM28J1PQzD+EJVMCCg6CkSMqDOtsmVq4BG1asQKED5IXAoQVWEFWlflyjPI5igplIqJNj1fB8R+FRVQG3EYKQSqvf5nsI0NQJdZ9xxKJSq7LcoQiYb8i+6LXWccojOzDmNXLd1A4MVwegQnL5/HVf+bRDTjmBHTUzDRE5MxkgJOuFYoKdMpNAADaUUQqoJ8l5J4AuEJggChS41hOshRPg3KtBAgNQEmq7xBYO/UOha+OylruH7Abqm4XruhLcnw/UKQrBAKfQgBFYCMKxwDV3HRbd1RFUiLBk6+0G4Z9yqywdvv8n0OTMpVD1K+QxCVdmc8SlWXNINzezq68GtFkN5DDOCHolQyVWwDJt8eZSWWC2WnaKuuZVYqh4RSDRdYFkxPNMkImyU8sI9YxgTAA+B56LrOpoMn5dtWROELuB5YVQRBOH/uW4V3w8P2C1jHn4QHnJMeNMAz29QfJIQXNJSYOEci629LgkBf16xieHBEU6dGeGsR3ZSDRR71Equ3UvwbD/c9nh54oAAS+hohkk6GeMvvSW+7Ev/0t5YSlxN0lDbTDaf57Slc1gytYtvPvQvmtJRcnmPn76hOHyGycUHBGwa9Fi+dpiXvvo7DuzQaEtFqW81WffhBm7/fJw1AwrT1BFCohkaUc1GMySmaSEkE0AQAmPMThP4DprU8d0AdBNNagjl4RMQCB3dMPF9hTRMfN8napn4wYRc6QRI+b6PQCJ1EebfBJhmDOVUqbcijHqKSWlJIZAYuoH0fSxf4egCPRIJvRUpMWQolRB6pSFDt9JC/l/H88B1kSL0jKSuEXgT4K0UMtDAMPE8J1QMlAJNyi88Jc2SBG5oxIZhomsCodkEvhfqdiQsxiplSo6D8F28wEdgAD4KjSBw0YQgqskw9+aFwCulAiVRKDQxwehsmJQDD1uLIfHC0FlKPF+hC596KYklba5/tcojZzTSP+TyUs8gfX0pTLGNnTmffFDhtGNncNUj24jGYpimiaYboWyDJtEAoUmCwEBO3KnUJUqJELRU6PVJIVCaQpMTlXDfR7fNsFXI9xC6INCN0EvSTADcwEVDA11DlxKBIBABge9jWjau6yOEQDMtfM8jFrFxqg6W1Ah8l38H3xqgGy4IEXqXQkwkQcLwOrAU2/sGmNzdwfjIGIauodlR3PwwDfVNKGWGRS1dkBv3iUQsnIqHaSewFSjPR9MlgZBEogkEUC0Mg4qjmwqkhqZrSGFg6KGAlSYECA0VeBMHi0QTYgIIPaTU8H0vHPn0fcpOOHrp+T7Vapkg8NE0+UW6QAkBE8zpvQXBL3ok19R7TIt5zD+qgRVvjbKur8hxH2T4zhzBKXPgxR2C+3ek2TRS4piFXazaPkomM0Z752TGMzuR0kJJ8b8TFKOm/KeOIJGqJRGJMamzm3f7Kry8cT0L5synmBtFAanA58XPhnn5c41z9onzlzMFy98q8PGugJmtAdmxIp8MS9bsFJhm2F0kNY2IEcWyTaLRCKZhIISiWnHRjLB1JJ8bD0eWfJ9IMo5umjhVj7Ij8L0qQkjiUsfDx5E6XlAM6Zc0PTQ4XcM0TRQBpXIVyzAIEOhaCEa+61OUktqqg/R8fEsDGaDbNpbvkbZsXF1HhT9G/Nvb03Q8P/SYfcLJFOVUkLKKGY2gPPWFh6dLcANF4IendjSSwDQlCkGlUkYIjcB3MDSNarWKL4yJMC4M/ww7QsQyEULiSgNfgBweJKpLLEsjVwQ38CeEuBS+604Ympzwwjz+nfPWpQ4ioOqEpK+VSpWkbjHqFWEih2XqJgqHWCAwkxan/HEnd5/UiihlWD8YJxJx2NDvsNeiJq55fICKrCUVt9E0GaYNhJhQQ/RDwJtIKSghMHQtTIkEasLoJVLqKM9BamFDuucHeGUHywgIPIUUHjFd4shQv9Z3HTRphGGvr/ADD13TiVoGSlj4foClmyEnoQJDGihPoUmDQPkYuoFmaCgl8D0PLwgQukQTMtwbhjEBnBA4FXaN5ZC6hWUnQt3qfC+aEcEydHzlgnKpejqm5lCtVDEMDbQqrp6kUqmEea/qKEO9VTS/Gc00CERAKt1AEOzE0G3QJD4arlMlEYngBy5SM9E1DTUR4kdsG4XAdRw0LIRSIeDHE/ieG4qwxRKUK2V8z6VaDeU1QpkDHwXoEsZduPIdxd3HVtixyyMVC+33L4dLhBNwzVtxVu0KmN1is9vsTrYHOukmCy0Wp6mlHanKJCMGypH/O0FxNJsXxYALF0yfc4+ZakBGaiiVxqmrixONRKhJpcJQI/BgdAQnUNzzZo58TnDtKc1c+eg4Zx3Vwmm3bqUnp2OZoRHEojHqoxLbtBkvlKlks3iRCPpESN1S30Y5N84PrzoDYfo0d7aTHclx1Q/uoaWlmeMOWEBNXYTaZIopU7og28OdT6xizWe9YUim5IQYkI7j+xhSEo/FESogVdvI5o0bkEJiSJ9Uwsao0cmO5Bkb90FJ4vEkNZ1tlPN5dMOgu2sq/T1bGRkaImInCfDIFooIKWnpqKehtpn+3j6UrmNqJoERhl52LIpSBsWxEb5y0BxOOXkpv/j1o3z8WR81NTXU1tTjOGUqeQ/TilLMVWmqCWhqqmXD1mGqnk8mXyYrQ8+6vWsSTiGDMm0iMsBzHBw3wPE8dKFYsPdUFu09B0O6/PFPr5EdLmLa5kS+TRB4AUJXCCkRmk7ZV3S1J5g/bSqpVIT+nVl2DmQYHy/juj5BqczUZIxLnx1mllVB2zXCeiOFj8fd/+ilpraWaCykXotGomGeD4mPwA8UuUIZt1rF1CUNTXU4bhXlehimHWoXux6gMZYrISmTsCQLZjZx+vHzmTqnmx29/dz8m+fZvnGEtnaboUoVS9eQygdfofQIslKknM+S9XzQLfx/5+KCgJq6RsxI+Lll4GPokQlJ1wAhQ60e247iIxDCRQmJUgoNgRd4SM2gWPUxhIZpaEilsOJximNZHCdMC1Q8ge9V0A1BxLCpekxU/01cpTDNOLlMP42N9Ujhg4himFGkhJQmKZsRRjMFOltt6uvq+HBNH+lklIgVrpuPwjKjDI9m0IXH/BntROJRPvh4E+VKBcfx8HxFTVM9ulLY0TheEMo4SEPDMG3cUolMZphSuYAMApBw6fOKXy11OWSWZPV6n2pVcP4bNodPraOl2aVm6jwSiQRCalQ9SX1NHYlUkl0xxer3PqSh1vrfCYpJW44dOK29ZnJ3HQNuiu19g7R3zWKwbwczps0gV8ySzmeJGBabtm0N1cikYPmn4DLCbu02F925hf6iSTQaFhp03SA3Ps7hM6dz2JJ5HH3GuaSmTMct9hLoDcTjSS45/5s8tvw9lj+X4oUVDxCMb2P5X19h11AGt5gnu9d0fnz1tUDAzy+9lr/86RnSdXEcsxZNBRiaYHAww9Rp7URi9ezq34GtSTTDwC2O8trKP5CvFpg1Yy7RVAqJT9UZZ3SkH4IqPTuynH7OLbQ2N1DJ5/jww3f5+fmHcMDXT2Dnzj5yhSJ1DfW0tNSxZeM2jjruWmqbOonaFp7noUlBXWMjlWqFU4/bk3POWUZjYxSwOfSIJWzePMCDDzzGk8+8Tt8ul/nzpmNJj5XPXM54dpxJre3EG6BUqlDeNUIm61LV4vzmjn+w8s0yMQGTJ7cwbVYzbVNaOOyrB9PW1kAiFQOGCEYrXPbdU1i58kPuuP1p3n9vE/XJZFivVmGVY0xZ3H3b6Rx/5GwME/5w7/OMjI/ys+sOZMnS3Tj3gr/w2qtb8QOfxS0pVu8SPLKxypFmiY+zBnV1aWwrgqkb2LYFmkRNhJ/RRB2zOy1+/qNlBJrHO29v56obn8a2TAw7hlI+QgpsPcmZJ8/huK/MYmxwgOaWFI6Mk7CqxNL17LXvVI75yr6s+XyAH1z9AP0f7qKuOUXVD6hp6uDggzrp7Iizx54LaWisRTd9dFUkV2lGBRXWrO3h8h8sZ1J7I6NjQ3iuQ0NrB8ODOzA1n4bGGPFkipnd9bzy3hCFfB63lAtbpswIUyfXctCiFlKRCnZEJxGPEfgGzqQUZk2UaI1OYrwGz3MxDA/p+dQ3x+nqqGXjZhfX8ZBSYUdi9O8awlWKI46Yy47BHJY0ccbHmbH7TC678BssPXQ/ysUCg0MFnnr2LR7843P07xpnxuyplAt57rvraupq6mlqn8yUKTOpVItUSjsZG81SKJS578EnefGVjxBOHl1Z6LpE6iblagU7FqczWUsuN0Z2bBeO4+B6Lt97JWBxo8+iNsVFr0NLKsLUA76K6N2KmW4mXd+IaSZpbWnGMC2qhSFSqov9ZjZwy4PP/U8UmP+Tqs//l1rWfecfrKKTplPb1E4BndquPakYMbZuGWbPGd3cec+ttNfV8as/3EsQVHGrRQxdw/UCTlqY4PEPK0SjGlKGYbNpmuiajijksVyXWt/j2FMP5ceP/IGglAPhM23qkcQiSYYH+9jcuwLT0Lny2zfz6NPv0trSzJ57dXLPQ39g5RNPcebpV1Df3kYyrnPd9Wey1/xOpBljoK+HOQvncfIpv2FktEC5kMP1PDI7e1ikwRFNCdYXK6Hmsakx55i9OOmmK4AIoLP+sx1c8K3fEg0c3HKOQzZ/ii4NDnjwJvb76kHglUEGnHrCVbz2Xj91DY0TLTgapqVTKJY59dg5/OSWW0D1gHD55eV3cvSyA5i9ZCng4/sVXl/5d067cDnXHzsVd/Va3tgyhKMUTXGTs85YyrTTl9I6pQlUHOJJfnf78zz4u5XM7a5j4PO1JBCYrk85FsXDp742yumXncSRJx4IQoPA44ln3uK6S++n5EwUHYRgyeG785d7TmVkpMDaTcOcu+xmFpo6bxervLzy2xx76v14vkFzzKTG0tiYKbGtUAr7PmvSROwovjBJJm0cN8A2TQQKhKSrPcazK28gMzTMt0/5CYvndvHjP75DTUfXRNuQj2ck+dm3FzO2ZSvL//RPSrvyYQ8gkAQaBNQu2Y2bnr0KTUXQ7Sj/ePF1rv/eIwgjQs/ATm7cdxKt+y7kvn99RNTxOPeso9jzhCNJN9r8u6Ly6SfbOfXcu5EKnFIOv5jj5iuP4YSLT8GwJOABMZ54+BGu+9UnVHP9SMPAKZYQapwdm29jtHeQZLwGoyHNUE+OaDpOvLaF/p4Kpx73fUxT59Zfn0ZnVyO6kLhOjndXDfPjGx9n8dIlmPYwN/zwG2TG8yw9/DImNbWx+7z5nHz8bnTNaKNarvDxx9uIx0zmzJtJvKaVXMFn05YdXHLFXQx8/gGtysHyoFBy6O5q5OhTlrH4+EOYtdfejOdL1KaaOPvsy/hw7Q6cSiGc/fYcgkCFhcdAoBs6xVwWt1wgqBSpej67NfhsHxek0i2YTTO593c38s9XV3PYkoUM5hyklSIIdD5e/RFqfAMjm9dw0VF7Mf+8G7/0pOKXdgOHzmpW11z+dTJunE39I5RyBTb1DNFUU0t713S6Jnewbes2Xn9vNSvfXo1bHkeKIGy1CQQRTQMjzDeFXxqWFUUKHdM0SMVMBtd/xsN//BEHnbwMhM9lF/6IN1//lPH8CJ9vfYbx4WGefuw5br31n5Cq4TuXHcXpZ55Fc9MCOjpn4qkAo5pj445+erY+RrVUREpF27Q5vP7GZn7y44cZ29WHpksiTpFyOce0RXNZ/ugdZLZsBJXhazPOZuGBM7j+73eGutBawOvvDPHj79/JyMggLekke+zaQdvcLr7599sY2bKRmvY25s85i7KsJxK1w4KJYSCCAB/BnMkGjzz1AJBlV7/LoXMPpatU5fJHf80hxx0ElNiyYYjZC8+mOxYQranh3Y13o+X6yQ73s/yXr3DvPa9zz5PXsOjo/XFy43iRJFdf8kc+++AzhkfGWDCrg7q4JJ20sYUk//4nPLVxlPO+expX/Opigtw4UlN88P4qlh13N0JYeKbNW69eTtoUmBHFwsU34ZZ0GqI6ajzHoXPq+OPHWaKWTlvEpN9V5KVB2XWwDYmpdNoaLXbffRIv/Gsr6do0QbUU5vuSjRy8dz233vsdvn3GjYy9uoGEpfFuzTTy+SwE4BFQKYxDdhfxWIRAtzlhyWzmLZrE9pE8XqHKpFQNK371N9rmdfDL13+LXimBp/h063ZOPOwXNHW2MW/XFg756lLWWhEYGmCvZJJH7v8Hl79xJ4v2nYVfKqAJn57hAl858QEqpVGacGjbuIHDbjqP8y87gkqximFFMGpslp38e9as7UcoB98PGB8eZMPaX+CNDdI4qZkbr36SZ+9ZQQqILpjCg0/8kkVLLsdA4eDy+Qe3MbxrCM+vkEzGuOuu98hkEvz2gTvZ/PnztLTWMKvzKAoyztQOm5XP3ULZ06kUi8ycdwaOr7jv7mv5xgXnhCG5aXHSVy9g1ZoByvkRNq1/Fdcpk9u+hQuWfB3Dh2U3X8VpF5wLQlBTM4nFe+7LrnEHpEQXEy1ZgYdmxiiVcpRLRUS1iOuEjD4FP6CrawapWIpTTzyBs84+hbFywB2/e5DNmzdSDqrEomGP6MHT65jX1cBfn1nBn15a/aWDovxykFjw8oZBYVUqYMKU9g6SjXU0NTTx3Ovv8Mjzf+fZZ59l0/rNHL73Aia3t1KXrkP5/gRrCiAlUsowMS5UWE2bUHsLvCKVUoVqvJbHnn2dML6THHn0gTiOTzxqo5txrFQry772dfxyjtGRDLvvsRfr1nxOvqIwLBMNRRCp51sXnkRtfT2t3buxc0QgpM1++y8kcIaJRa0JYfeAdDzJ2/9aRbGcx6pxee3NVbwjJK+8/jlvP/MW0k6AkaStRWfLwEY038Mt5HimonhgWz9BeQwzFkczbObPn4lhmBhSw7AMbMNEs2yEphGzI188y1y2SDlaT3T+7ryy/GnAACwGenuIRWLYXXvw0ZZB1q0ZpFRwSDZNZcXmYdzmDs498zf4OhhRm2isA9/w2Towwlg5YG0mw88fv48fPvwTLrrjBu4acqmdPInnbv0r9/72CWSyjmoxx8K9d2Nmdy26Jlg4r5HO7kY0N0N+dJTXV17NMScuYOtIAVXbwJOfZYmakknpBAMeONEYuqXT3txMQzzFlZfvxxsf3cgtd1zEqg9/Tnubjl8sM6OlEeGNst9B8wCbTzf2s6N7Gq9GW3EdjyndszEtk0Q0Rl1dC8mOaXh1U7BT9ax47HU663V++F8H8uPbL+P2p99mVdsUtmwd4bqjrkWmu5DxOAt2n8vj/7wVvxzwvFbP8l0ZbrzjJm587KeM7r8HVlMtTyy5lH88tRLNlBCNsH3LGPldnyGUYBc6H3fMZO8j5lIa7sMXFQQF3OExzvv6HuSrBp7jEbUjlCs+xUKZ5t1259KLH+Dxh/4BU6cy3jaJobU97Dn7VHxHEW+eQnY0Q9WXNE2aTkvHHEplwRVXHU39FBNIsG3TJpRyKPoeyUSSLVuHQEiUW+S1195HGhG6J3dz449+jxCC7MgYmaEh3n13DfFIjHLBYevmz8kXi2SV4h0RYa1usnFbL3rEIl3TzndPPIHExk0YmiDwPNwg1DoXSscrlxAK9MAn8KqoQFLxPVoaGmjqmEJz92wW7T6TO376E771rct55oXHeO/DVfiuQ3NtLfW1CfzAZP+Tv8ajr30svkQ/7csFRUU4TXDr/U/QYTqMDfVheoJPP/uMnYNDeIUsA0NDjI/nyI47zGxtoq6mlmi8Ds8H29SIGDq2YWIZBslIlLhtErV1LMPA0Awcp0LUNvjzc6+Tr/j4SmefA49gtFxh1pyZCCOBHUtR295KY1OSSZGA7ul78OQTz9DWMQmUJBAGVcfn9NMPx0y08O77O9j7wHMZ6CthGAFXX3MBkWiSqG1Tm6qhJhGlqSZFLJbAStQzed7eoAIu/u0FHHLK3oxt/5R7b3+IubNPIeoKDAFV10U3DAYGOvrufwAAIABJREFU83huhUIhSzU7hAhcKuVK2EOnCTSnTJPK0+5l6d05GrLNAqVSBuW69JSq9Dr+RNjmUyy5WEIhNahvbufJR14kWtdMMFFFVTUxnnjpGmR2iJFMmbNPu4xnHn0JTQosQ9G7aYCx4V4ELoO9W4l7khFHo6GtgfuuuJ0P3v0EKxFHyCT/dd0FZMdyrF+1gavOup2d/SPUNybAy/Hbnx3HmvU/Y79jFuAYMSKRODsLFbx4Ak0KNGBkOMvNvzqJS689h78/9TmJ9Gns3NHHcytvZcmRM1nf08eqDx7hpK8fBgQ8+cSveehHp/Prby7lwpPm0LP1c5LxFHYkBpqOZkRCbkBpYje0EmwaANNkZPsGBnoyGFJnfbyeTW+v42fHfhslXZQTsPuiNubv2U5S0xgezADjgMlr/3iPvq45TD3mIPY7YH+UHuEnP/4jhx5+OZoeIV+skMuUMWzFbl0JitJkj+mXsrW3DCrg6K/M48xls0g1T8b1AyZNaqOmsQNkDRVH55WeN1m19hne2Pwiy9c9z0N/uomTjtwb4VbJlxVFT2fVa+8TqUuRjsXwqgHXXnsqxx67mGefewVNawhdDV1imFHiiTSaHqW9tQVfWejROFK3UYGDGY2Qqq0nGo1hmjamaTE6kscydcoll8mTm3ll7bvcevevCQKfuy/7L4KVr2B1deP6YBo6ugzZ5T1NISwjHGQwDNBMPAFWvIHps/egra6OOa1RKuUqqz5Zy8bt29jVN0B2tI9NGz7F1BSV8TFmt5gcd8jRVF3vy00mfpmg+O/r8TWDoinlM6M5glcsoiuPFdefyaGz2rnwhINYOL+LYqHM6UcfQGdcY0prMwqJ5wf4vhcSo2oGuhVBNyxMXScWNTF1DSEVpgblqsvGdWvQdJtEMo1tKqbN7gZqKedyBL5k8SEHsNcR+5JMxFj+8DN0tHfR1dnOnJmtTJmUYN7eCwH4dPVqzj/7ZHq2fARujsOPPoJU2kRKg1ylSrZQRtgWoAh8hW0apFtqOfn8s8j399G/Yyffvfp+uidNRY8mUEKSLVWQUqexLomMNVBf14YQoTA4ipDfsOKi4bFwRjtzd++iVMhPgJ/Cd1wqroPrOGze3guqCqpKR2cbXuBTLhSwTBunosBKo0fiVJXk3t8fx8xpLYhImr8//hpvrPiUdDKFkGAaJk2JBJZlgFchVZMCK4pySqwrQ9fUOqbPmEQQqQGrmaH+EZQI8A2Lp19Yy5LD7uLhP79LQ3cnxUJAbSzFb26/gGde+THZQhk/kUAKDSENjGicsUyGyZMbQGm89NKHRIG1H+0CFG+8tY79d5vMy3f8gWre4v2X1/DNSSey4ohvs+6Ht/PEbX8hLl0q1TJOpUhQLRF4DkK5OK5LYJrs2l6GdD1RHKSpEeSGKOfGKE7qYmarBX4eYZiAz6LpTYxlxvD9AMiCnyWailLo38Z5j99MLBXgF3byj6feZvrcfZg0ax8uOXsRv7/tVH5x4zFodpS3V7zF0QfO4tFfP4yIx3EzRX54w9cp7+qhrq6RVFSQrG0mKI7QP5xnYNNn7Nq6iXuuuIGr9l3G78/5AeuefS7seMBkPJPn5t89w/5TT0DW1GMbJm6+yLPP3ko6VYdhmSRicUQQYJsS369SdUr4fhnXddCkxDIthBbBtEzy4+OMZcfxvCpe4FPbEENoOpFYnMHtPUSlT7aYQwlJ21APfXMWsrYCmmUg0DE1i2gkDp6P73oo38HHR+gaUrfpmjad7nl7015jcMZZx7Nj3Wqshk72W7g3UyZ3cczBBzMlJojndlBnQvceC3jls+1fvov4nwCKAL//88uMuwqnVGBueyN92QwzIzkOOuU0Dl3SStEtI2Nprv72KbSnIsxobwlFiZRHxSlTLuTIjI9SLBbJ5PLki1UqjoPSbaSmU5NI8cbL7wAanjvAUYfvzX4H7svTD95GvLaJqutx7vkn8fNf3s7pJ52BMGvIjI4w3rOemQmDX3z/DEzbxi9nueQ73+Heh37J4qUHgWFTzW7msSeux/eKIA1GKx59/SOMDY8hpE7UlvTuHGNq1zG4TpbalMHGD2+moscxo3HKrkfc1Cn5PuP5Et74IH29m/CFYkfvTpABCoEuoK9njM49p7Kubxctk1onPMUq6Aq7WqEpauE5E3NtvkKTPjHTxA0CXEVYySUGWoQpnU2cuux3BIkO8BzOufx0jj33GHylI6QgaRsUHI9KYQT0CKXcMLYepijseJw7Vj1PIi6RJFn72idcc+UdpFNJkjUpiCdoaWnimuuf5eClP8aMxnGrJSh7xGyLYrVEJBpD1wSmZYZ9pBrE03WUKwmeeuEd6tINHHzUAQzthMGBHMWP17HlXx9hJdq4574n6Jk1g2cWLuK5GfuS6ZiPq8col4s4VQc/cPB8D88L0KSk0TCZPrkORBI/P07gV8O+TsvGb06x7J4boKqDGfC3X/yZ3970MNFoZKJYokMxS0L32FxwWLr/mfjCQPMq/G3l7QxnCriDn1L+60oe+9FD7HvgHEYHR6itq+Vnd3+d6/9wEX62DyIJIt4of3vqajauXs2D990AfgkZSXHb7d/myP0v4NKFy9j06irk5FkMztyX8dkHUnED7KjF6EiGUsmhXJY8fcty7Ka5IB2cwjA3/vwqVKCoSVr45RIN6QhmclI4MmvHMQwTCRRKZQhMDDOKYRloGrjlIiLw8dwqCkn3jG5qa2tZMmcf+j5bj0/Avvfdw3ZdA6Xhlkso5eIFLr5bDtufqmUC3yPwfHxfYUVizN99HxqSGocftjfju4ZYs3E7SdPmjGVH013nc0BkhPqoTWPSpNYWrH3jVf6Tri8dFG95arWoGR+luTFKoQwz95vMYZccAfk+nn/kdTLjGV5++W2aJ89gXrNkTl0ES9epej6672FrOhGpE4nEsAwdSUDUjmFbMYxYmnQ0yr9efw9QGGaC8795BksP3IMrb7iH3JiLoZvMXLgX46Of89yKV6ltaEE3DLKZIs898U+8nM+bL7zH/IYlnLPvQVxy/Bks7VzK9d+/DSvZiBIav7v/Wpyqh51IU1+fprahDl1o1Da30BGzMZyAu//wBs1drZQy45z3tZkUPJOq5yE0ScSIMHdWN/nxDJmhrfSsf5vulihBpczQ4BC7zaznlluO57Szj+aFf62m3WQidyhYuHcXt/3lGrRGm6dW3gWVLOiKqdPbKVcreL6LZSVQpSEY34qfG6ehxqKYk5y46GJ2ZobALfKrm0+mY3Yr+IpkcweqWsaKNoDyaWxpZMbsTtqmtvPnFbdQnxoFLc5vvv9TDj/4IkwzRtVxue/hS9lVzCJ1m9ltrfzghu+iJ9NEWrohMpNHfvEE8XgUXQub3yN2BEPTmVEfpammBctMM7e7g2nT6mmdNI2BLZ9hSZ3itDlM+9aZQJV1n/bgWwlcPYKpm+goEjGLqG2jggA0i1gsjmlamIZOl6fQ62N4PWtItLQilWI4U2T//Zt54elLoVrBjSa56YQf8NtrHiFob8O0InTUheF/UPHYd7/ZqEQr69f2cuIxV0EyQVODztmn7EVPf5btMk61ElAbrXLogddzwRl3c+60H3Be0wXc++inGLaLnmxh7rxGzjl9ERce/A0evuN5kBYzF+zDG2ufpHHZkcSW7Meyy07n6CPnkR8ZIuqWaY1ZTOnupCYdo1w/iQdv+xvXLD2JsX4XMz4JghxVp0TPrjEqhSKL990Nr5AlHqtht/m7sdf8SYxly3R0dIIMqFQKSCmoTaaJp+qIxePMmbeItrYmErE0Y2WXxJRpnL3v8Yx9+DGWbfDsE7+noSGJbZh4rovveriug/D+TTji4/vh2OakyVOYNWc6HdYQnc0pHr/3dgaK0Ne3hfc+3cjsrulU0VnYEceKJjlhQQu3LX/xPwoUtf+Iu6gWf3T0MYsZGiqw+dNB3nttK6vfeJudFUnZFdQ0NRHTFWdcdBYbX3mGXMlnR7aEJkUoqCM1ovEUtmmgCzHR2yZxXI9kTT1yfBdnX3Q20ghoSKcp5ItcfcPvaK3RWLzkAEBx7QXfITNawozGUErhWnGmTuvi0h9fy4n7HYVW10jPcJ6tOwYwo3FWvfUBF337MgzTp7Gpna5JCda/v4GlR+3PkkP2Bw1MO0E6lWDs8828+Px76IUyi4+azx6zGvh0Y4ZSRaclprPXPlO46y9XMr71UySQHxvj5JP35vtXncIFZy3h6OP2ZNKkBFs+H+aBB18ksq2X1Y89R7Hq0dDZwu57LeBrZx5NIuGRyzo8fOcTXHLC96jEa4knm4hGNe69/yLMRAz8MosOPIhP39mEIQV/uOZPTFs4gymz57DsxLk8/si7GFaMBXNbWbp0AYl0E1Y0zelnH8dZ5x1DQ32CsSHFt776X/z5r+8QS6RQSPZcMo/vfP8o/vXMe1B1uPupn7Bo/90pFTQ2fjbA90/6Fv98+T06Zs1F6PrE7LnCD3xSqszyn9yJtCS/vPdWvnb2MfSv28D9//UTBrU4PoJrb/wWpqV46v4n0ewYrguVShmkwHEDhKYhNQNNClQQGmrUtDluVg0HX3EMZgOgLM46fhbnnDCdb5y8ACOdIjNe5efH/4RVr29kZMp0PM+jJRXj3ocvRLOaMWIpZsybQn6Xg4g08/ZLb/LhB72ccPKh7LOonRefW0NvvJVvXHQwTzy0kk82jGFNmUMhYjLZ1pjZ0Iis72TdG+/jZBwOPepAejZsZfntj/PmY/+kfloDCxfP5KsnHsW82TN48Z4/seKhRwmiMY5f0MgPf3M+qeYO6kZ6cHpzDJo2Q72DvHbzA0TdIh377Mf6dZ9zx11/JR6L0RTVmNTdSmtnG/GaJOdfdAaV8UHOOe90uqZ0Uy3k+eTND3nt5TeJNHVxztcOZc7c3chlCviBojGh8dHWUUqmwUv3PMTue02joauD2d0dPP+PdzFNgaFpmHo4xeVNtOe4bhXbjrH0iCNZUD/AQYccxZbVL7Ft1KB3KItmGoz1b0SMDNCeDIjHBE4hSzIo8Nh7W8iWnBv/U0BR/CfcQl1Uu/nFn11+1etbBxkbKZEv+9TUWCHvnR9F1w3a6iOcee7p/O3nV7D67c9YNerSl6uQtHVM3SSersWL12ALGS6QaaNHY7Q31PH722+kefJMIAfk6Vu3iblzDueX5x7G+Q88C3j0vfYs511yE2NWAr/qYESS3PL9k/jLL+7ho6FxKvF6JOG0gikVfnmUg5qSxKsBbR3N5EezPLFpGw/cexWju4pMtQyyW3aw00rS17+ZXy5/hbaqQ1cqwfefvpz05NnsOfdKDpiV4vA57dzx+BsY+SJ2yScGlHTJdFNHlh18BWlLo1/X+GjqXGpwiOd30TI2xoasxzlXnMqyy07mdzfezZN/eYW2pEG1ayZ9foRidpQ/XraExx5/meG8S8FVNLbXsWTxPO598j1UsYTd08Old13BSRefygfvfsp3L7mPoW3r0DN5Fs/r4qB9ZrPse+dgtTXx8vLnOOubvyIaS6LQiUYjDOddbrnqq5x53TKgFtc1KO/czp3f/R0vrniXYqlM3bR2xgqSxqnTyBYKWFYEzwtDXU3TaTJcMhvWYMdSfO/cY3ngpvtoWDiN9anpeJVhXnrzef563dWsvvdJMnMXsj5fxfV9pAir866nqFbDNphypYgbgFMpk+5bT4thctzpe3PE2V9h8j5TQY1AOcPbT/Vy3dl3EG1vYmOsjcCpYgqfGdEi+yHQd2ZIRqMMVnxkY4y7ilFqTI2+jZ/R3d3Ey+88Sl//OLPmHc8F+/2f9s481rLsqs/fHs54p/fufUO9oaq7ph7K7R6qB9OGxgHklkUQTixsJ4AUQpQQEERkIJiQCDBhCAYyAEFRkKJEcSQSYTkewEFG0MbGjdt29Vg9VdWrud787njGPeSPU00bbGNjEqkiZf/13j13OGefvddZe+21vt9dfPzzF2jf8yZMVSOlIBptsnTlAuvCc1eqmSsNJof/0Q7YPnKK03HF9ksXOPnGYwwODfgvv/0kx4+vkasOG1cu0S4LdsuaKXDH2gCV5XT7S5Rxmzc9fDfpxz7GoZUjHPned/B9P/nrLM71iYY3GO9PKNOEv/b2r+d7/s47Of1N38SF567wP3/6fWx/+Pe4ISzPrt9OVuQcn4y5PJrxLglXHIy05o/bMb6/RmIzli5d4T3v/SG+9V/8G372X/5zPvq7n6XMhngvyKqaYjqiKgv2hwe8413fzZuOdVmzT/P49/9rfuGHf5iXJl0qb5BC0gkEkTKcDq+zV8LRuRZPPneZ33xx63M7s/Kh/28U/0z7kb960j/6dY/zmReu4IKYQeBodee4kEe0A4kTNW9+4ARvffx+fuq7v4dPvjLizM4MKRRRIGm3e8x3e2RxF+8scZRSlAUPHu/zzY+e5vd+8wPsbo8JyoKhrdnr9lmqZxivkYGmN9/m2e2MldUjhFJy/vpV3qgzhjVUy8eYzTKkrUiSgEEU0vYF1cEB9yaC0SSnFpJzQjHa3GEeuDuQHG5FTI3ljPFsdBdZiBTz0xF6lPETv/VewvUef+uv/BgqChg7gY9iQiUJtSY3hn6c0NYagaevNVetYzto49C0fMGCL1kXOZONy2xNKmZJTHxoiWnYZSxiwKPKMeHeddq9uQZ4ejDjvjtXeeXVS8zSAcV0Ri8OOZkf0P+Ge3jnD34HN7Yrfvof/wfWDi9gRrsUwwnLPqN0sFUrTNohjmOk1IynGbV3vC3NGbRSVu49xsbLV5hcvE40mGfS7nCdlEvX9xm0YuzyGlEQUFYlgdQI6QkEIEMQlsV8m6XhDjfW7mQ2t06V1bz3xx/nwz/+75g+f4Hynvt4iZBJViFUg1mrvaKuKoTWBGFIMZs1AAOt6LU61KPrVFvXiUdj1vot2q2IzfGMLK8pV48xCufA1WCbTYlBts+pSNNLE3IcVqfMfMnLwRKj/U1akx2GwzFBEPAbv/IPGCYxv/BDv8L2oaMIBzqK8V4QK0U03UEnKcY5lKkYhJJxUbMrE06sLfDmRx5g47NPkVnL3JETXNs4x7Ube8RpzGg0oiUFxhhcPmnQaUkHIxUPPnQvk3LIzlPPMr56nWr9MCOjaccRVoWobES+v40pMpKkze31lLsWF7jWW+IV3WZqPKKYcGowz3g84WSvzXQ4JIpCPpV7xmWBEJJB4HnH5nlu/4a38F0f+TgP3nsfSWdAWRUc7O9TljOyLGNheYXvfPtbub06w3f8wx/h0x/8LX7jQ5eoRIQVkkR6Di91aYU1cu8q3/vNx/mvH3uGH/voWcEt1m6pE3r/97/NP7sFhxZaFM6xaxM6i7fT66UEwjHX9ty1usCNF5/k9z/4IZ44n/PCxNCKNKHW9LrzKKkJOwOidhvjPPXGGeKsZLB2iFArjNBUUjOUKbKeAg0s1lc5trOMqWtagaJ0DjM9gPkVxnv72DpjaXmFVVkQzPaZ+QDXmmcsE8psjLWW+UAxzSbMyoo0aSGUZJYXRDpAekeoFVlVs+xLVq9t8u5f+xFsZ56f/cUPs7W3hQwClI4oqwJnHGESEydt4iQCJKaqGkZhGOK8Q9QlOgypi4zA18xMw2SMWy1q43G2RglQKqQqMqJ8wum7V3jh3A32xzkKh5QgpWI+0fQnY17ZGfITv/6POPPKkM9+4hW2r10Cb+imMQ6BdA4RBozHUyZZgfKSpJMSRwG9esq8q2nPd7lRKcJWi83RlP1RAd6y3k3QSytYITHWolSId42KXxRFTKsaghZhKyWIW5w4epjHjszY+d0neeqZDSa3n+QgbBGGEaPxCCkVUdTGeo9xDZygqht2YG1qwJMkKdPJFGdKYi3o1BNiHCPrqdMBFZoo0JiybGqIvUdK3eh3S4mWmoXBMmU1Y7x3jWp/F4FD1hWtOmdlVPJLv/z3+KUPf54nLo3x6TxaK5RuNjiEK/DFhDTpkedZA+2tK5CKu4+tcPqh+7i2tYU2lhrF1Vdf4mDzgBP3PEhdTdkf7lOXNfuzDKxFCY/SmpNvOEk5m1Ebz/WN81AacucwQmDR6EAiLMTaE3uDCCJ2rKJSMUpAGERNuV5V0WklZFlOVRWEUYSzdXOOHtI05GQv5G0XX+TluMfq33wX/+mDn8AUM7JsRJHnZFnB49/4MN/+4BwPPXiU3Y1z/OGZjDO7Ed12wjSvscWEhUTwlgdvh80NjqWGb/n3nxaT3NxqNvEWiSnebN/zrfM/efpITC1C5paWCI3lzQ/fj006uHiOuN3mTacWOf7A13Nw9vdYixwXdkr2KgiVopjNSOKQxQCMqYiShIkMCdePEna6uM6A0jqqsIUMQwhTSq9wQQLpPFpwExfVEFl8kJCN9lhd6lEWEw5F8NgdXZ4/v0NLW2odEoQxRZE1Bi/P0FLhRaOhHEmJwoMUdFstat9AXtP+PHppgf33f5CXN67x9PYunaRFN0lRuuHcSa3o9PoEUpKmHcIwvgkLrbB1RV2VSATOWpxzWJpkdqUT4igmkDRUFu/xWc6Cdjz0wDp/8KmXmNWGSCvUzTgcspE6v1I5TjzwMM/95kfZef5pdmSbanZAOwoASRyAFaLZbUTS6vSY63epipxsNmNUe67mcHFUMM5zdoYFZWVuFsYJUmVpL6yx7RRWqQbdlXaoggSnIryOUUGIrS17w31GL32C6Uc+wWWnubp2nJ08R+sAYxxFVeCMwXlLWdWo1zh/UuKdpy5nqCCirguUlCglMQbqqMNYJVjdxiuJspZAS4xpRLO8dyilkcITKI1zNe12ynwnZnzjCqIsEN4RDg7RVo4j6wmvfOCTvHRti/lBhyzuIm7W6iupcKYgwLF05BSjvet4oUCHhKGik4Ssry5S5lO8d41G8851cIpub55ZOcOUJXhLKn2TDpN0cN7RThOyYoI1FfODHkQhSkmsg6TbZ2IEgfAo1UguoDRGR4ggQnlQoUYIQbuVQgNOR0qPNRUKUEISBAKNx0chFxeWmbmS3/nAR5ChIhYOUxuqqmR50OPvfvt9PHRPj6Pf+Le5eObTbJYLxOk8aafLXUfX2Nu6wfH1HneeOkJX1OijA37tA0//FLdgu6WM4jPn9n/y+04H6K0rLMVDQgr8jeeQF/+Qevci2cXPcunMJ3n1iQ+xfOIukuICYlyzMXYI6YlDhTcltakJTUHoMqpsyurqCh3G3Hv/Izz/ykv00ohWrOlEiuV+i+noAFmXCFujpEcHTWrKwnyHLMtZCEuKvOTb7u9jigmjYYnqzJN0EgSOwUKf/b0tDi906EWCXrtNNw1YmEuJAsF8GhFpz1yqCaUFUzdo+BNHqaYH9LpzeFOTJpLAe1rK0RaStrSErialRJYzEuVJdEAioBOFTIZ7yDrH1SUhliKf8k33HmPnYAh1QScOmdOWNxzr42zJp598GR0GBEq9DqW1njAOQAiWDq0Q6YBzuxN6y+uoOGU2HoOEVhTidEQQhIRRQhTF+DrnYHcPY02D+AeEdDcrjQK8b/qyKV0WCO+IsEyQaCXwQtHtdslmObUxlHWOM008zjtLZGu2F2/noL2AFjVKByRJi6KowFtiHeKtJY4U/e4c2IpIKSKtSIKAbquLybObsFSLcRalA2bZjDBJ0ELS6c2jgwCBJAgCXG2xNKlNQgqiMAGbMT4YY+qclU7I4fU1ct0mWlhlMarZjUPm+ylCapZ6Kee29omShKTbZ3Vpjd3rr/LAvQ+xPz4ApQniCOMFqws9Dq/cTj4dUZQFVS2Y7O0TKoVQKbaeYAwNRmySocJWI4GBoTMfUdQ10gmE1Cz0U9bX+8xJw/WZYeXoPVRCoKMUpTylSiDqUBuDCiOiuA1SkcQpnf4CFst8bw7vHXGsSJOAQbfF0kKPuRAiV6JaKcvrq3DzIWywZEXFu79unbeciuiu3sl//Cf/jJd3BBf3C5TN2bxwlovnztIPKkQxZvfceR5b0/zMx87x/Lmdn7oVkrVvCUrOl2tnrxXio9sjv3iQUBWeYQWHl3qktqCvDGsrMYuLPYIqYXM65NDpB/j803/Io33JizNoJ4pOHBBqQTdRLCy2GE5DFvshqooI3YzVQ0to3wjWR0lEFGrk+iK7O/tURU0QCZT2iDCm12nRObbCbPMC0lrSRICOWep3sN0uaTth4hVJFKDLDitzMf1ORFZALcAasD2Nt74RTZeC2UxRVQ3wNk0VIlojVhq5NSLudagqgxAOZwVhHCNlQBBpSquorGuSnhFILfn73/k3qARsXLnMZDJivd7h9J1L3H+4x1OXbxDjuLLlePqZC4yGM8IobKjeDQi6icdREwYhtatZXV5i4+yLeG84mGaEcwl1XYNXjG1OqA0CR2EcxlR4IfDagg/RWuOdRxpBg4RsqNevyxJIahUQdvqsLx0iihOUDrGTXVaOLpHlJVY0ifBBHDOdjJHhHchyStxOiSm4eHWbdtrh1JEjjLKcKE5I2/PEgeG5Vy9xaHmN+cEy0/E+0+kEU5fIbodEeZwrSIKQKNT0F+5gNMup6ora2gau2+40Cc5SIIWiFcd0Wy1UEHHhwlmG2nNb2mKx0+HwsduZXciQOHTnLh69ewdfW4YTy37m+O7TS2yZNkYHrCzOcWf6KM9+/n/xjre+m888+3l02qWuaxZ7mtoYpIxQgSSVAhkpgtIhpMPWYMoKU1bgQAaaLJuysNhjNpzhrAEVIFyNjwNwNfV4ymxccOf8PNLMmEwrolCg0DgVNNo5OqAdRxhbk8QhrSTBVAlxFGFMSYAlCTxpGtIJFaExeBWC9JQ1KKVIheNgmPPm4wPuj/c5fuQufvU9/4o/uOaooj1y50m1QggYJIqdkWDsPa1JxtWHl/lvv/O8AHcrOoq3VkxRIDjRV/4/f/sSdbzGszcswxms9FLiMMCLgqNHEhb6js1hTqza3Lh4gd///Q1eyiXaek4f7nLyUIcH72xzeHWJ7lyLJ851NYJ0AAALrUlEQVRc4uu/7W3s7F5k5fRjbH36DEUtUUlMP22DcwQPPsJvv+/nWFxbRwtHGgqGO5vceXKZf/vB5zFBys//2g/wRz/3qxx/w3EiBe1Wnxvjknw24+XL23zLcc1nNsacvzHj6ggW52KUavRb9kY5de2QOqSoIath8yAnd562Viz0QtbnFAJPFCvCOMDWDic1t508TG0kWZYRxjEH27tsbk/pRY4TK32kUBRoOoPDnL14g4+fP6DX7vHi2ZcZjjOCuIHJIgXSWexN2QIlFDqOG+8vjRjECdevXQWpqa0jTROGB3s3i/896iam3klJLCUVHqEUOgooZgXeOaxxBKHGAUIFDZ+QBo7bTiPuPrGEkx6cxHpLr9sjDGJimRO2WggfM5lOKWrH9Z19rLEcXl+gqD1eBox395BBSBLHpEmK8h4nJbPhPlIKJllOHCdM85wiy6mspT/XpdeOMWVJaWpsVWOsYTLLwXsyY5DOksYBi50OxnlE0KT3zKea0WzCVg4PLWuU8aSDJUZ0G43qqE3LjWmVmxTZjF4r4OyFXQarR5DxPPNpzKmVQ/jJdY7cdZrPPPsCGsON3W2mq3ehojajg31mVYEvLZevbBCJkDAdkI23mGQzROkwrqI7WOPq5nWOnVhmurOFkRqpFVGY0u1GdDuKa8+fZ2Uw4JF7lvnrb/5mPv7k03hXsbs/ZLEXYZ1lNitII4nCcftSH1/nPLNxrXlQ2pqVVHJby7GZC+ZSzTSvWejE3BgWPHdtzG5heXFrxk4Q8PgyvOX+Ba49dZFP7Dp8GhHEIc46giYexaiwODTH+jGP9QQ//sd7/NFmLrhF2y13Yt0Y3vtW7aNZDy1TRrki0JJ71lM2ZgFht0O74zm5ZvAHQ1rC8tQzV/nsiyUf3PS8sScYtGMeWG9x350DVgYxo5lhZS6kFQgOJlOWVvrMKs+0MCRpizwT9BZTXCmIlSVemUP2A7ACLt1gkuUEb7yX8IWzyMMrTDa2sECsJMNJzsXtGaNJzVo/xMRtrPDsj6rGsDlBXdcIPKNhQVZUREqiFExyx+Z+zqCnqSvH6nLErLA4L/FaM9dN2Lg8QgkYLLUpak8YxPRamu2tCShIWylPnz/glaEnXV3H+YCLL51jd3uXINJ41WD1G7UngXM1UoY4XxMEGhlEhEjqOqPX6jIajzC1I2nP42VFNZwAnjC4mTAvBdILZBThsFgvKMoS5yxBFOBtjaklYaipjEZI04g+BZpjK/M8eHcfW3parRhvDaYyzKchg7ji2tRx9NAhrFBU0wlnz28SaEk39uxMS3pRRFbUHD08oChrep2YdhoxyQv2D3Lm57ts747Z3DrAOMu0dKSxQiiJlBFBKEgCydb+mPvvWGc0nhIHgu1RST4rme9F5FaRxrKRFRBwZBAQUrOxlbO9X+J8Tac/z1waMp1lrCx0WOwmuHyKmx6w0BakrQ6ViahsSGYsSinm4pB+aDBOIsKQzMAT6hSjzJBnE6yvmO6OuHLtEku9VXIvKGcjxpMJUnhMUbC0uMS5a9c5deoYu9evIIIYoUICVdFKUgb9kBfPnOfxu1d5212L7IynFFaw1Ekp65pJXuKdYFyVTbVXHBJQMTzYox9IhnlBpBSplmwfTG6uJeXNMnvPbil4ZViyOan4xNaMNx3u8JYjKQ+riheu7HO5DtgTmm6kGNeOQx0JteeykUQe5rUhN5pffHZ4yxrEW275DDAu4Ic/bMXPvHHkL+cZ3ghOdgNerqb0WyFVMcGVMbMgoR0lfPyZfe6b67A5V/JoJbiaQWZKNg5Krm5NefPhiH474Hefs1wfVjx8vMML58b05kL2JyWBaCbw5U/N8DLk3mNzPPnxszxxecZFG7A0l/KmuxeYfvoJPnfugLp4nu9/+ykeua3N7vU9Zld38bnhDcsx01lGWIxZXU9xiaeuDMYaStV4ULYPQjcbIsWsZiRLTg80gXLoQNA+GiOsQLVi6sKyuZXz8uyAmdSs6YhDS10Ekr3tPXQ75cVtw8VrhkosMPIZF55+FZtloBVhFGHxN0nhTQ01NOtmYStQkqoyhEhagwE7OzP2xyNiHdKKFDaQiMpilGo8ReXRWmJqg1KSvCpA+EYcynuEChsadhTf3KAwJEnQkLYdzUZRnrGQWS6MK+ZEh66Gw/2ISNeIIqOaTqmvjXju8gFPXRlyfr+i9J5EQKQk1nvSULFw9jxH5iJy6zh5qA0OTqz2GNQVo9E+cVEwrSyH04CFJMQbx34+JvYBw1FNWFgenas4oOT5K0Mm2zlT45gcKDqJYrs0DTxYesyOIpGQKsHAO4z1jK5f5XN7BUdShb8hmASCFE9Ve6JBgG4NiQPJShoyFJqFdkxkLecvFlw4MIRK4VSE/LpHKLevgM1pxTFjYakKg14OSPDkuUZrAVaDKxFCYb3FAd7WOBWArwhFjfeCYpYzLkrCbIgZCdy0YjguKPck07xkVhkOippIKtIATKSwVc3+tOJMWdISinYoibB413hM09LilGSndGxMDRPvuTCz3NcPmZeGw3nB+y+OuLeneGzFs+NgVHjKWDCpPZGDfGZw2tGtLL/80vSWNoi3pKcIjWToYsT+P71Dz29MBN4J5gI4kgTcP4i4VgpMHDCrBUPjCKRmXQx56mpOaeDJkedTUy8kIEUTyDX+S199O9TU1lKbJu5X268t8Ktu8j2UAC0FzjdaHs436Ia/eB9wU0byi9tCuPDfB93WOweLbepihjWeuixRuiGW4CyV9Vjvm8eebwSK8J5Qq9dJ2cITBZpWdw6CmIPNq/Q7LXzcwu5tUYmAVqdFPs0oPVhXN0JQtUEKhxYSdIAWklFRgCvpdOeobRML81KhpMDWFUmaMitmnN/e+aLQ+mteWe2/3BD96u5J8AWiR957jH9dd+4vE87XQqAVaCWIA8XutP6qPhcKqHwzNuJAkNf+T6Jo95+6g+9693ex+cpZBBVhd4Ur589y/sI1Tt11isLXzCY14+Ee3pTks5yV1cOcv3KR++69gxtXLyJU0KT/CEuapnRDweeeucTLe3tiZuyXnOj+zzUF/iseeeOc9o8tR7RDQeIh9I6eUiwEjn5HgpGsdySvjgzXMkfmHT3puS30/MBzNecybtlY4i3rKb42oLcL0f+5l8zPf+8R9aO5F2xmEo1hfN3gJAwri/WSxRQuzTzPekFPR7wwM3w2qwWNLHkj1fllf0gwLV83Wc76r5GELpody5u/97Ua1j/dB1/+2F69965gUnlfFiweXqbKcmRbI5WiqiowEiVrJI2SnkeCrRr9Z9Eo3EnnqB0kcYTSnrqq6IQRKo3AWHSnw+qhQ2xv7jZL8CJHSUWVTwiDECUC4iQlSBJ8nVEXkKMQwpMmEaNaoJXB+4bMbK2gF6Vfsm///Pvkv2Lfv/ae2n3x/fs/Mf2M980usPFMS/dVm+rq5hssMPszFv+29TVuXH6V0XiPRCtkr4l1IgRWOGxVY4XDeQFOo26WrQaxxjiD8QGJTBo9Z2qMkzjXqDH+6b5szvQrj0j/VR15fmRFr6P9SmFwOB5JPHe2PFFpcBNFOxZUheRoAB3lm9zQ2PODz9acy9wt7yXeskbxtVuxV/Ge950z7/lSA/C11/5ycg7+LzwFv7aJ+xf3lF+7xtf+9v71Ce+BzWosKm9/Xm4FPzq30G2gn0qAMAQBRAiqukQFEmscTjVypBiLwSAFhFoSpjGmMngc3aUBgfDUomb5ttuY7uySFRl1VeK9xDvHfNTCBgGhUlgaik+UdKiMp5oOybKSbjdAaxC20cVuto8qgqjNfJgwrIuvetniv4olTSMdelP43r/+Of8Fnuhrr33Z90j1J/99YV+LmzKef/Yuyy91bje/3H2F4aAE3PuGN7B96QKBsPigy3Q4pK4qkiDC2aoR6ioyvHOUxYwoUFSuopUkFEXZPDS1QmLxVmKNYWqhrmpq7/6vjU3vPZ+8MvqTS/7AF1y/EPXrfepf7xvH/1vtfwPeb8xX1hSFXAAAAABJRU5ErkJggg==) 7px 13px no-repeat; }\n"
       << "\t\t\t#masthead h1 {margin: 57px 0 0 355px; }\n"
       << "\t\t\t#home #masthead h1 {margin-top: 98px; }\n"
       << "\t\t\t#masthead h2 {margin-left: 355px; }\n"
       << "\t\t\t#masthead ul.params {margin: 20px 0 0 345px; }\n"
       << "\t\t\t#masthead p {color: #fff;margin: 20px 20px 0 20px; }\n"
       << "\t\t\t#notice {font-size: 12px;-moz-box-shadow: 0px 0px 8px #E41B17;-webkit-box-shadow: 0px 0px 8px #E41B17;box-shadow: 0px 0px 8px #E41B17; }\n"
       << "\t\t\t#notice h2 {margin-bottom: 10px; }\n"
       << "\t\t\t.alert {width: 800px;padding: 10px;margin: 10px 0 10px 0;background-color: #333;-moz-border-radius: 6px;-khtml-border-radius: 6px;-webkit-border-radius: 6px;border-radius: 6px;-moz-box-shadow: inset 0px 0px 6px #C11B17;-webkit-box-shadow: inset 0px 0px 6px #C11B17;box-shadow: inset 0px 0px 6px #C11B17; }\n"
       << "\t\t\t.alert p {margin-bottom: 0px; }\n"
       << "\t\t\t.section .toggle-content {padding-left: 18px; }\n"
       << "\t\t\t.player > .toggle-content {padding-left: 0; }\n"
       << "\t\t\t.toc {float: left;padding: 0; }\n"
       << "\t\t\t.toc-wide {width: 560px; }\n"
       << "\t\t\t.toc-narrow {width: 375px; }\n"
       << "\t\t\t.toc li {margin-bottom: 10px;list-style-type: none; }\n"
       << "\t\t\t.toc li ul {padding-left: 10px; }\n"
       << "\t\t\t.toc li ul li {margin: 0;list-style-type: none;font-size: 13px; }\n"
       << "\t\t\t.charts {float: left;width: 541px;margin-top: 10px; }\n"
       << "\t\t\t.charts-left {margin-right: 40px; }\n"
       << "\t\t\t.charts img {background-color: #333;padding: 5px;margin-bottom: 20px;-moz-border-radius: 6px;-khtml-border-radius: 6px;-webkit-border-radius: 6px;border-radius: 6px; }\n"
       << "\t\t\t.talents div.float {width: auto;margin-right: 50px; }\n"
       << "\t\t\ttable.sc {background-color: #333;padding: 4px 2px 2px 2px;margin: 10px 0 20px 0;-moz-border-radius: 6px;-khtml-border-radius: 6px;-webkit-border-radius: 6px;border-radius: 6px; }\n"
       << "\t\t\ttable.sc tr {color: #fff;background-color: #1a1a1a; }\n"
       << "\t\t\ttable.sc tr.head {background-color: #aaa;color: #fff; }\n"
       << "\t\t\ttable.sc tr.odd {background-color: #222; }\n"
       << "\t\t\ttable.sc th {padding: 2px 4px 4px 4px;text-align: center;background-color: #333;color: #fff; }\n"
       << "\t\t\ttable.sc td {padding: 2px;text-align: center;font-size: 13px; }\n"
       << "\t\t\ttable.sc th.left, table.sc td.left, table.sc tr.left th, table.sc tr.left td {text-align: left; }\n"
       << "\t\t\ttable.sc th.right, table.sc td.right, table.sc tr.right th, table.sc tr.right td {text-align: right;padding-right: 4px; }\n"
       << "\t\t\ttable.sc th.small {padding: 2px 2px 3px 2px;font-size: 11px; }\n"
       << "\t\t\ttable.sc td.small {padding: 2px 2px 3px 2px;font-size: 11px; }\n"
       << "\t\t\ttable.sc tr.details td {padding: 0 0 15px 15px;text-align: left;background-color: #333;font-size: 11px; }\n"
       << "\t\t\ttable.sc tr.details td ul {padding: 0;margin: 4px 0 8px 0; }\n"
       << "\t\t\ttable.sc tr.details td ul li {clear: both;padding: 2px;list-style-type: none; }\n"
       << "\t\t\ttable.sc tr.details td ul li span.label {display: block;padding: 2px;float: left;width: 145px;margin-right: 4px;background: #222; }\n"
       << "\t\t\ttable.sc tr.details td ul li span.tooltip {display: block;float: left;width: 190px; }\n"
       << "\t\t\ttable.sc tr.details td ul li span.tooltip-wider {display: block;float: left;width: 350px; }\n"
       << "\t\t\ttable.sc tr.details td div.float {width: 350px; }\n"
       << "\t\t\ttable.sc tr.details td div.float h5 {margin-top: 4px; }\n"
       << "\t\t\ttable.sc tr.details td div.float ul {margin: 0 0 12px 0; }\n"
       << "\t\t\ttable.sc td.filler {background-color: #333; }\n"
       << "\t\t\ttable.sc .dynamic-buffs tr.details td ul li span.label {width: 120px; }\n"
       << "\t\t\ttr.details td table.details {padding: 0px;margin: 5px 0 10px 0; }\n"
       << "\t\t\ttr.details td table.details tr th {background-color: #222; }\n"
       << "\t\t\ttr.details td table.details tr td {background-color: #2d2d2d; }\n"
       << "\t\t\ttr.details td table.details tr.odd td {background-color: #292929; }\n"
       << "\t\t\ttr.details td table.details tr td {padding: 1px 3px 1px 3px; }\n"
       << "\t\t\ttr.details td table.details tr td.right {text-align: right; }\n"
       << "\t\t\t.player-thumbnail {float: right;margin: 8px;border-radius: 12px;-moz-border-radius: 12px;-webkit-border-radius: 12px;-khtml-border-radius: 12px; }\n"
       << "\t\t</style>\n";
  }
  else
  {
    os << "\t\t<style type=\"text/css\" media=\"all\">\n"
       << "\t\t\t* {border: none;margin: 0;padding: 0; }\n"
       << "\t\t\tbody {padding: 5px 25px 25px 25px;font-family: \"Lucida Grande\", Arial, sans-serif;font-size: 14px;background: url(data:image/jpeg;base64,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);color: #E2C7A3;text-align: center; }\n"
       << "\t\t\ta {color: #c1a144;text-decoration: none; }\n"
       << "\t\t\ta:hover,a:active {color: #e1b164; }\n"
       << "\t\t\tp {margin: 1em 0 1em 0; }\n"
       << "\t\t\th1,h2,h3,h4,h5,h6 {width: auto;color: #27a052;margin-top: 1em;margin-bottom: 0.5em; }\n"
       << "\t\t\th1,h2 {margin: 0;padding: 2px 2px 0 2px; }\n"
       << "\t\t\th1 {margin: 57px 0 0 355px;font-size: 28px;color: #008467; }\n"
       << "\t\t\th1 a {color: #c1a144; }\n"
       << "\t\t\th2 {font-size: 18px; }\n"
       << "\t\t\th3 {margin: 0 0 4px 0;font-size: 16px; }\n"
       << "\t\t\th4 {font-size: 12px; }\n"
       << "\t\t\th5 {font-size: 10px; }\n"
       << "\t\t\tul,ol {padding-left: 20px; }\n"
       << "\t\t\tul.float,ol.float {padding: 0;margin: 0; }\n"
       << "\t\t\tul.float li,ol.float li {display: inline;float: left;padding-right: 6px;margin-right: 6px;list-style-type: none;border-right: 2px solid #333; }\n"
       << "\t\t\t.clear {clear: both; }\n"
       << "\t\t\t.hide, .charts span {display: none; }\n"
       << "\t\t\t.center {text-align: center; }\n"
       << "\t\t\t.float {float: left; }\n"
       << "\t\t\t.mt {margin-top: 20px; }\n"
       << "\t\t\t.mb {margin-bottom: 20px; }\n"
       << "\t\t\t.force-wrap {word-wrap: break-word; }\n"
       << "\t\t\t.mono {font-family: \"Lucida Console\", Monaco, monospace;font-size: 12px; }\n"
       << "\t\t\t.toggle,.toggle-details {cursor: pointer;background-image: url(data:image/png;base64,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);background-repeat: no-repeat; }\n"
       << "\t\t\th2.toggle {padding-left: 18px;background-size: 16px auto;background-position: 0 4px; }\n"
       << "\t\t\th2.toggle:hover {color: #47c072; }\n"
       << "\t\t\th2.open {margin-bottom: 10px;background-position: 0 -18px; }\n"
       << "\t\t\t#home-toc h2.open {margin-top: 20px; }\n"
       << "\t\t\th3.toggle {padding-left: 16px;background-size: 14px auto;background-position: 0 2px; }\n"
       << "\t\t\th3.toggle:hover {text-shadow: 0 0 2px #47c072; }\n"
       << "\t\t\th3.open {background-position: 0 -17px; }\n"
       << "\t\t\th4.toggle {margin: 0 0 8px 0;padding-left: 12px; }\n"
       << "\t\t\th4.open {background-position: 0 6px; }\n"
       << "\t\t\ta.toggle-details {margin: 0 0 8px 0;padding-left: 12px;background-size: 11px auto;background-position: 0 3px; }\n"
       << "\t\t\ta.open {background-position: 0 -13px; }\n"
       << "\t\t\ttd.small a.toggle-details {background-size: 10px auto;background-position: 0 2px; }\n"
       << "\t\t\ttd.small a.open {background-position: 0 -12px; }\n"
       << "\t\t\t#active-help, .help-box {display: none;-moz-border-radius: 6px;-khtml-border-radius: 6px;-webkit-border-radius: 6px; }\n"
       << "\t\t\t#active-help {position: absolute;width: auto;padding: 3px;background: transparent;z-index: 10; }\n"
       << "\t\t\t#active-help-dynamic {max-width: 400px;padding: 8px 8px 20px 8px;background: #333;font-size: 13px;text-align: left;border: 1px solid #222;-moz-border-radius: 6px;-khtml-border-radius: 6px;-webkit-border-radius: 6px;border-radius: 6px;-moz-box-shadow: 4px 4px 10px #000;-webkit-box-shadow: 4px 4px 10px #000;box-shadow: 4px 4px 10px #000; }\n"
       << "\t\t\t#active-help .close {display: block;height: 14px;width: 14px;position: absolute;right: 12px;bottom: 7px;background: #000 url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA4AAAAOCAYAAAAfSC3RAAAABGdBTUEAANbY1E9YMgAAABl0RVh0U29mdHdhcmUAQWRvYmUgSW1hZ2VSZWFkeXHJZTwAAAE8SURBVHjafNI/KEVhGMfxc4/j33BZjK4MbkmxnEFiQFcZlMEgZTAZDbIYLEaRUMpCuaU7yCCrJINsJFkUNolSBnKJ71O/V69zb576LOe8v/M+73ueVBzH38HfesQ5bhGiFR2o9xdFidAm1nCFop7VoAvTGHILQy9kCw+0W9F7/o4jHPs7uOAyZrCL0aC05rCgd/uu1Rus4g6VKKAa2wrNKziCPTyhx4InClkt4RNbardFoWG3E3WKCwteJ9pawSt28IEcDr33b7gPy9ysVRZf2rWpzPso0j/yax2T6EazzlynTgL9z2ykBe24xAYm0I8zqdJF2cUtog9tFsxgFs8YR68uwFVeLec1DDYEaXe+MZ1pIBFyZe3WarJKRq5CV59Wiy9IoQGDmPpvVq3/Tg34gz5mR2nUUPzWjwADAFypQitBus+8AAAAAElFTkSuQmCC) no-repeat; }\n"
       << "\t\t\t#active-help .close:hover {background-color: #1d1d1d; }\n"
       << "\t\t\t.help-box h3 {margin: 0 0 12px 0;font-size: 14px;color: #C68E17; }\n"
       << "\t\t\t.help-box p {margin: 0 0 10px 0; }\n"
       << "\t\t\t.help-box {background-color: #000;padding: 10px; }\n"
       << "\t\t\ta.help {color: #C68E17;cursor: help; }\n"
       << "\t\t\ta.help:hover {text-shadow: 0 0 1px #C68E17; }\n"
       << "\t\t\t.section {position: relative;width: 1200px;padding: 4px 8px;margin-left: auto;margin-right: auto;margin-bottom: -1px;background: #160f0b;text-align: left;-moz-box-shadow: 0px 0px 8px #160f0b;-webkit-box-shadow: 0px 0px 8px #160f0b;box-shadow: 0px 0px 8px #160f0b; }\n"
       << "\t\t\t.section-open {margin-top: 25px;margin-bottom: 35px;padding: 8px 8px 10px 8px;-moz-border-radius: 8px;-khtml-border-radius: 8px;-webkit-border-radius: 8px;border-radius: 8px; }\n"
       << "\t\t\t.grouped-first {-moz-border-radius-topright: 8px;-moz-border-radius-topleft: 8px;-khtml-border-top-right-radius: 8px;-khtml-border-top-left-radius: 8px;-webkit-border-top-right-radius: 8px;-webkit-border-top-left-radius: 8px;border-top-right-radius: 8px;border-top-left-radius: 8px;padding-top: 8px; }\n"
       << "\t\t\t.grouped-last {-moz-border-radius-bottomright: 8px;-moz-border-radius-bottomleft: 8px;-khtml-border-bottom-right-radius: 8px;-khtml-border-bottom-left-radius: 8px;-webkit-border-bottom-right-radius: 8px;-webkit-border-bottom-left-radius: 8px;border-bottom-right-radius: 8px;border-bottom-left-radius: 8px;padding-bottom: 8px; }\n"
       << "\t\t\t.section .toggle-content {padding: 0; }\n"
       << "\t\t\t.player-section .toggle-content {padding-left: 16px; }\n"
       << "\t\t\t#home-toc .toggle-content {margin-bottom: 20px; }\n"
       << "\t\t\t.subsection {background-color: #333;width: 1000px;padding: 8px;margin-bottom: 20px;-moz-border-radius: 6px;-khtml-border-radius: 6px;-webkit-border-radius: 6px;border-radius: 6px;font-size: 12px; }\n"
       << "\t\t\t.subsection-small {width: 500px; }\n"
       << "\t\t\t.subsection h4 {margin: 0 0 10px 0;color: #fff; }\n"
       << "\t\t\t.profile .subsection p {margin: 0; }\n"
       << "\t\t\tul.params {padding: 0;margin: 4px 0 0 6px; }\n"
       << "\t\t\tul.params li {float: left;padding: 2px 10px 2px 10px;margin-left: 10px;list-style-type: none;background: #2f2f2f;color: #ddd;font-family: \"Lucida Grande\", Arial, sans-serif;font-size: 11px;-moz-border-radius: 8px;-khtml-border-radius: 8px;-webkit-border-radius: 8px;border-radius: 8px; }\n"
       << "\t\t\tul.params li.linked:hover {background: #393939; }\n"
       << "\t\t\tul.params li a {color: #ddd; }\n"
       << "\t\t\tul.params li a:hover {text-shadow: none; }\n"
       << "\t\t\t.player h2 {margin: 0; }\n"
       << "\t\t\t.player ul.params {position: relative;top: 2px; }\n"
       << "\t\t\t#masthead {height: auto;padding-bottom: 15px;border: 0;-moz-border-radius: 8px;-khtml-border-radius: 8px;-webkit-border-radius: 8px;border-radius: 8px;text-align: left;color: #FDD017;background: #331d0f url(data:image/jpeg;base64,";
    os << "/9j/4AAQSkZJRgABAQEASABIAAD/4gy4SUNDX1BST0ZJTEUAAQEAAAyoYXBwbAIQAABtbnRyUkdCIFh";
    os << "ZWiAH3AAHABIAEwAlACRhY3NwQVBQTAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA9tYAAQAAAADTLW";
    os << "FwcGwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABFkZXNjAAABU";
    os << "AAAAGJkc2NtAAABtAAAAY5jcHJ0AAADRAAAACR3dHB0AAADaAAAABRyWFlaAAADfAAAABRnWFlaAAAD";
    os << "kAAAABRiWFlaAAADpAAAABRyVFJDAAADuAAACAxhYXJnAAALxAAAACB2Y2d0AAAL5AAAADBuZGluAAA";
    os << "MFAAAAD5jaGFkAAAMVAAAACxtbW9kAAAMgAAAAChiVFJDAAADuAAACAxnVFJDAAADuAAACAxhYWJnAA";
    os << "ALxAAAACBhYWdnAAALxAAAACBkZXNjAAAAAAAAAAhEaXNwbGF5AAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    os << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    os << "bWx1YwAAAAAAAAAeAAAADHNrU0sAAAAWAAABeGNhRVMAAAAWAAABeGhlSUwAAAAWAAABeHB0QlIAAAA";
    os << "WAAABeGl0SVQAAAAWAAABeGh1SFUAAAAWAAABeHVrVUEAAAAWAAABeGtvS1IAAAAWAAABeG5iTk8AAA";
    os << "AWAAABeGNzQ1oAAAAWAAABeHpoVFcAAAAWAAABeGRlREUAAAAWAAABeHJvUk8AAAAWAAABeHN2U0UAA";
    os << "AAWAAABeHpoQ04AAAAWAAABeGphSlAAAAAWAAABeGFyAAAAAAAWAAABeGVsR1IAAAAWAAABeHB0UFQA";
    os << "AAAWAAABeG5sTkwAAAAWAAABeGZyRlIAAAAWAAABeGVzRVMAAAAWAAABeHRoVEgAAAAWAAABeHRyVFI";
    os << "AAAAWAAABeGZpRkkAAAAWAAABeGhySFIAAAAWAAABeHBsUEwAAAAWAAABeHJ1UlUAAAAWAAABeGVuVV";
    os << "MAAAAWAAABeGRhREsAAAAWAAABeABBAGMAZQByACAASAAyADcANABIAEwAAHRleHQAAAAAQ29weXJpZ";
    os << "2h0IEFwcGxlLCBJbmMuLCAyMDEyAFhZWiAAAAAAAADz2AABAAAAARYIWFlaIAAAAAAAAGwPAAA4qQAA";
    os << "ApdYWVogAAAAAAAAYjYAALdyAAAR/1hZWiAAAAAAAAAokQAAD+UAAL6XY3VydgAAAAAAAAQAAAAABQA";
    os << "KAA8AFAAZAB4AIwAoAC0AMgA2ADsAQABFAEoATwBUAFkAXgBjAGgAbQByAHcAfACBAIYAiwCQAJUAmg";
    os << "CfAKMAqACtALIAtwC8AMEAxgDLANAA1QDbAOAA5QDrAPAA9gD7AQEBBwENARMBGQEfASUBKwEyATgBP";
    os << "gFFAUwBUgFZAWABZwFuAXUBfAGDAYsBkgGaAaEBqQGxAbkBwQHJAdEB2QHhAekB8gH6AgMCDAIUAh0C";
    os << "JgIvAjgCQQJLAlQCXQJnAnECegKEAo4CmAKiAqwCtgLBAssC1QLgAusC9QMAAwsDFgMhAy0DOANDA08";
    os << "DWgNmA3IDfgOKA5YDogOuA7oDxwPTA+AD7AP5BAYEEwQgBC0EOwRIBFUEYwRxBH4EjASaBKgEtgTEBN";
    os << "ME4QTwBP4FDQUcBSsFOgVJBVgFZwV3BYYFlgWmBbUFxQXVBeUF9gYGBhYGJwY3BkgGWQZqBnsGjAadB";
    os << "q8GwAbRBuMG9QcHBxkHKwc9B08HYQd0B4YHmQesB78H0gflB/gICwgfCDIIRghaCG4IggiWCKoIvgjS";
    os << "COcI+wkQCSUJOglPCWQJeQmPCaQJugnPCeUJ+woRCicKPQpUCmoKgQqYCq4KxQrcCvMLCwsiCzkLUQt";
    os << "pC4ALmAuwC8gL4Qv5DBIMKgxDDFwMdQyODKcMwAzZDPMNDQ0mDUANWg10DY4NqQ3DDd4N+A4TDi4OSQ";
    os << "5kDn8Omw62DtIO7g8JDyUPQQ9eD3oPlg+zD88P7BAJECYQQxBhEH4QmxC5ENcQ9RETETERTxFtEYwRq";
    os << "hHJEegSBxImEkUSZBKEEqMSwxLjEwMTIxNDE2MTgxOkE8UT5RQGFCcUSRRqFIsUrRTOFPAVEhU0FVYV";
    os << "eBWbFb0V4BYDFiYWSRZsFo8WshbWFvoXHRdBF2UXiReuF9IX9xgbGEAYZRiKGK8Y1Rj6GSAZRRlrGZE";
    os << "ZtxndGgQaKhpRGncanhrFGuwbFBs7G2MbihuyG9ocAhwqHFIcexyjHMwc9R0eHUcdcB2ZHcMd7B4WHk";
    os << "Aeah6UHr4e6R8THz4faR+UH78f6iAVIEEgbCCYIMQg8CEcIUghdSGhIc4h+yInIlUigiKvIt0jCiM4I";
    os << "2YjlCPCI/AkHyRNJHwkqyTaJQklOCVoJZclxyX3JicmVyaHJrcm6CcYJ0kneierJ9woDSg/KHEooijU";
    os << "KQYpOClrKZ0p0CoCKjUqaCqbKs8rAis2K2krnSvRLAUsOSxuLKIs1y0MLUEtdi2rLeEuFi5MLoIuty7";
    os << "uLyQvWi+RL8cv/jA1MGwwpDDbMRIxSjGCMbox8jIqMmMymzLUMw0zRjN/M7gz8TQrNGU0njTYNRM1TT";
    os << "WHNcI1/TY3NnI2rjbpNyQ3YDecN9c4FDhQOIw4yDkFOUI5fzm8Ofk6Njp0OrI67zstO2s7qjvoPCc8Z";
    os << "TykPOM9Ij1hPaE94D4gPmA+oD7gPyE/YT+iP+JAI0BkQKZA50EpQWpBrEHuQjBCckK1QvdDOkN9Q8BE";
    os << "A0RHRIpEzkUSRVVFmkXeRiJGZ0arRvBHNUd7R8BIBUhLSJFI10kdSWNJqUnwSjdKfUrESwxLU0uaS+J";
    os << "MKkxyTLpNAk1KTZNN3E4lTm5Ot08AT0lPk0/dUCdQcVC7UQZRUFGbUeZSMVJ8UsdTE1NfU6pT9lRCVI";
    os << "9U21UoVXVVwlYPVlxWqVb3V0RXklfgWC9YfVjLWRpZaVm4WgdaVlqmWvVbRVuVW+VcNVyGXNZdJ114X";
    os << "cleGl5sXr1fD19hX7NgBWBXYKpg/GFPYaJh9WJJYpxi8GNDY5dj62RAZJRk6WU9ZZJl52Y9ZpJm6Gc9";
    os << "Z5Nn6Wg/aJZo7GlDaZpp8WpIap9q92tPa6dr/2xXbK9tCG1gbbluEm5rbsRvHm94b9FwK3CGcOBxOnG";
    os << "VcfByS3KmcwFzXXO4dBR0cHTMdSh1hXXhdj52m3b4d1Z3s3gReG54zHkqeYl553pGeqV7BHtje8J8IX";
    os << "yBfOF9QX2hfgF+Yn7CfyN/hH/lgEeAqIEKgWuBzYIwgpKC9INXg7qEHYSAhOOFR4Wrhg6GcobXhzuHn";
    os << "4gEiGmIzokziZmJ/opkisqLMIuWi/yMY4zKjTGNmI3/jmaOzo82j56QBpBukNaRP5GokhGSepLjk02T";
    os << "tpQglIqU9JVflcmWNJaflwqXdZfgmEyYuJkkmZCZ/JpomtWbQpuvnByciZz3nWSd0p5Anq6fHZ+Ln/q";
    os << "gaaDYoUehtqImopajBqN2o+akVqTHpTilqaYapoum/adup+CoUqjEqTepqaocqo+rAqt1q+msXKzQrU";
    os << "StuK4trqGvFq+LsACwdbDqsWCx1rJLssKzOLOutCW0nLUTtYq2AbZ5tvC3aLfguFm40blKucK6O7q1u";
    os << "y67p7whvJu9Fb2Pvgq+hL7/v3q/9cBwwOzBZ8Hjwl/C28NYw9TEUcTOxUvFyMZGxsPHQce/yD3IvMk6";
    os << "ybnKOMq3yzbLtsw1zLXNNc21zjbOts83z7jQOdC60TzRvtI/0sHTRNPG1EnUy9VO1dHWVdbY11zX4Nh";
    os << "k2OjZbNnx2nba+9uA3AXcit0Q3ZbeHN6i3ynfr+A24L3hROHM4lPi2+Nj4+vkc+T85YTmDeaW5x/nqe";
    os << "gy6LzpRunQ6lvq5etw6/vshu0R7ZzuKO6070DvzPBY8OXxcvH/8ozzGfOn9DT0wvVQ9d72bfb794r4G";
    os << "fio+Tj5x/pX+uf7d/wH/Jj9Kf26/kv+3P9t//9wYXJhAAAAAAADAAAAAmZmAADypwAADVkAABPQAAAK";
    os << "DnZjZ3QAAAAAAAAAAQABAAAAAAAAAAEAAAABAAAAAAAAAAEAAAABAAAAAAAAAAEAAG5kaW4AAAAAAAA";
    os << "ANgAAo4AAAFbAAABPAAAAnoAAACgAAAAPAAAAUEAAAFRAAAIzMwACMzMAAjMzAAAAAAAAAABzZjMyAA";
    os << "AAAAABC7cAAAWW///zVwAABykAAP3X///7t////aYAAAPaAADA9m1tb2QAAAAAAAAEcgAAAmQUAFxfy";
    os << "qwIgAAAAAAAAAAAAAAAAAAAAAD/4QDKRXhpZgAATU0AKgAAAAgABwESAAMAAAABAAEAAAEaAAUAAAAB";
    os << "AAAAYgEbAAUAAAABAAAAagEoAAMAAAABAAIAAAExAAIAAAARAAAAcgEyAAIAAAAUAAAAhIdpAAQAAAA";
    os << "BAAAAmAAAAAAAAABIAAAAAQAAAEgAAAABUGl4ZWxtYXRvciAyLjAuNQAAMjAxMjowNzoyMiAxOTowNz";
    os << "o5NAAAA6ABAAMAAAABAAEAAKACAAQAAAABAAAEsKADAAQAAAABAAABKQAAAAD/4QF4aHR0cDovL25zL";
    os << "mFkb2JlLmNvbS94YXAvMS4wLwA8eDp4bXBtZXRhIHhtbG5zOng9ImFkb2JlOm5zOm1ldGEvIiB4Onht";
    os << "cHRrPSJYTVAgQ29yZSA0LjQuMCI+CiAgIDxyZGY6UkRGIHhtbG5zOnJkZj0iaHR0cDovL3d3dy53My5";
    os << "vcmcvMTk5OS8wMi8yMi1yZGYtc3ludGF4LW5zIyI+CiAgICAgIDxyZGY6RGVzY3JpcHRpb24gcmRmOm";
    os << "Fib3V0PSIiCiAgICAgICAgICAgIHhtbG5zOmRjPSJodHRwOi8vcHVybC5vcmcvZGMvZWxlbWVudHMvM";
    os << "S4xLyI+CiAgICAgICAgIDxkYzpzdWJqZWN0PgogICAgICAgICAgICA8cmRmOkJhZy8+CiAgICAgICAg";
    os << "IDwvZGM6c3ViamVjdD4KICAgICAgPC9yZGY6RGVzY3JpcHRpb24+CiAgIDwvcmRmOlJERj4KPC94Onh";
    os << "tcG1ldGE+CgD/2wBDAAYEBQYFBAYGBQYHBwYIChAKCgkJChQODwwQFxQYGBcUFhYaHSUfGhsjHBYWIC";
    os << "wgIyYnKSopGR8tMC0oMCUoKSj/2wBDAQcHBwoIChMKChMoGhYaKCgoKCgoKCgoKCgoKCgoKCgoKCgoK";
    os << "CgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCj/wAARCAEpBLADASIAAhEBAxEB/8QAHwAAAQUBAQEB";
    os << "AQEAAAAAAAAAAAECAwQFBgcICQoL/8QAtRAAAgEDAwIEAwUFBAQAAAF9AQIDAAQRBRIhMUEGE1FhByJ";
    os << "xFDKBkaEII0KxwRVS0fAkM2JyggkKFhcYGRolJicoKSo0NTY3ODk6Q0RFRkdISUpTVFVWV1hZWmNkZW";
    os << "ZnaGlqc3R1dnd4eXqDhIWGh4iJipKTlJWWl5iZmqKjpKWmp6ipqrKztLW2t7i5usLDxMXGx8jJytLT1";
    os << "NXW19jZ2uHi4+Tl5ufo6erx8vP09fb3+Pn6/8QAHwEAAwEBAQEBAQEBAQAAAAAAAAECAwQFBgcICQoL";
    os << "/8QAtREAAgECBAQDBAcFBAQAAQJ3AAECAxEEBSExBhJBUQdhcRMiMoEIFEKRobHBCSMzUvAVYnLRChY";
    os << "kNOEl8RcYGRomJygpKjU2Nzg5OkNERUZHSElKU1RVVldYWVpjZGVmZ2hpanN0dXZ3eHl6goOEhYaHiI";
    os << "mKkpOUlZaXmJmaoqOkpaanqKmqsrO0tba3uLm6wsPExcbHyMnK0tPU1dbX2Nna4uPk5ebn6Onq8vP09";
    os << "fb3+Pn6/9oADAMBAAIRAxEAPwD52wBxTW4BPtTw25CTj8KikycVzI62BPI46HNGetCk7cUuMg0CHAYX";
    os << "pSHtxUmcgYph7UIGNbihhkfSlakNMkchxj6VJgbeaiWpFfAx2oZSG4zmkI+YU896YeCKBMa33j9aUcm";
    os << "hvvmlXrxQIRe9Ieppyd6b60wGnp9aaODSn0pBTJHkcn0peMgd6a3ByKUnmkMP60q9Rk03I4pV46mgB3";
    os << "IoPTPpSHoc80hOFNAxxGepprfdBFBPH1prcAUCAdcdqdnqB1qEuSfQU5XOOelOwidRlaixhDUseSKac";
    os << "7CD0pFEBqSM/u85qN+KcP8AVj61RKHE5JoTBpozTkJFIYZweKU880AZ5obgYFACDtStwM96MdKa55xT";
    os << "EOQ5FKetMBIPSlQ80AKvSk7kUL0Ipe5oATODUnHJ9abxxRuJJxQCE5JpNuQKUn0oJwopMaIyvIpw9qB";
    os << "1FIMknFMQ/HOaaeR70p4puefamAN1PvUbjPenk5qN+nFCEJj3pQeTSfw0meaYhR1zTScULyaGoAUc5p";
    os << "6HpUS9alHHahgiRv4aRz81Ix6U1up+lIY5etBHzGmp94fWnZ5NAg6EZ6VJtG0YqMHkU/dkCgYw85pg4";
    os << "apG/pTMfNQA4H0o9aavFOU5HvQIWkFBpKBscaKOnFJntQA7+EjNJ2xSbsLml7UwEUfOKGHzmhetK/Wg";
    os << "A7Yp38OM0g6Um7K5pABoHFJnNL14pgIe9Kab0pw5oAPSkb3oY4FNbmgQHlqeOMUzHzU8f0oGP2jac0z";
    os << "GScdKfuwKjJ5NIBQORSN1ozyKa/3jQIenLUqn71MX7w+lKp6n1oGhH71G3GKeeaibrTEOBzSnrmkWhu";
    os << "DQA8nkU3HvRnml/hoAVBjvUi9R7VGnTmpAcUhijpTsc5pmeR6U4ZNAwPNNC8mlOQRmlPU0gALwaXkUo";
    os << "OVOaQH1oQ2PwOvNMzkmnbiCM0EYzigTG9xQ3SjuKG6AUCFHWh+AaRzzSEknpQMVeRmkPehDzinetACD";
    os << "gZpM5PNOHIwaCMc0gQj4oBwaRzmkoGLIf3ec0wUrf6s/WmpzTESYyoqZhheKYAdgA6U6TgUhjM+vWmn";
    os << "rjtTWc446Ugcg+oosImX7pzSgY6GmLyDTgfl+lIYo6A+tKcnvTeqilHQY4oGI3U80f0pG56GjI5oEO7";
    os << "kd6bjkelAPJoXk80ANNKOlIaUdqYhw6inN/DSdxSv/AA0igPBNIv3h9aVvvc+lC/fFACgfMaXAGKQck";
    os << "08dqQ0LgAcVE5znHpUjPkY7VGw4NCGxqjH40o5PSkHFKnNBKAHr9acy5WkHen5wDmkykRZ6UmeSfWnY";
    os << "wB3prE7cUAOXkA+1PwDwaijyM1KW2oCMfjQxoaF+Xik2/dJ59qcrfLRzkHHagAC8DpTjjFNc4xSg5Ga";
    os << "BCA9aQjgUp64obpQIa/GaXjNNl4P4UIcNz0piHYx+VJ2qQrkcdcUxxhM96B2HKcqc1EzelPH3SO5qPG";
    os << "DQA1v9ZUikg/SkI/eU4DmmJCRNkvmlHWiJQN9Ljk0gIm+8aQU5+tNFMQsnQ0fxA0jHINNB5piuP6mlH";
    os << "Wmg80oNIY8n1prEbaGPFNJ4oAVjyoobp1pq9TQTxTAiyCfc1Ioynqaj24PSp4BlCO+aGSiWMggDOOKA";
    os << "BsOeQKWPCqwIPTsKZlRAeep6UiyCUYXn8KX+FabKc4ApZDtiUiqJHL96nqM1DG4ONwx9KsRlWYAEUmC";
    os << "FH6UhFSkdcdKiYUhsYzYANGM4PrSNzxT9oGOKYhtNUkZNTY7e1NCjHSgBqng04D9aaw24p69OKAEPBF";
    os << "PXC5460jZHJoLKPvECgERvgHjvRn5RTJJAMlRmljbdHnvmnYLiqMuMetKBzxwM4zSRsA+DSJjB570CH";
    os << "khSRnNMxj2NKTnAHakmwAPc0AIOhqNqeajPUU0AZxTQfmNDGmE8mmIkTHFI1IpxinYoAROoqTvUS9ak";
    os << "7igEOY/dobrTX6rStzSGIv3hS560g4ajvTAd6UtIThRTkHy0AITTf4qcw4pBwaAEHTmkXrThQo4JoEK";
    os << "aBQe1FIBG6Ug4oPNHrimAo56048ikUZ6048CgY1eCKVupoHUUHqaAAHFIeOlPHI5pjDHSgBp5pymm+l";
    os << "A45oEOIoWlFIOpoAa1BpzjgGg8UAN/ipynrQ3JpVGBQMSk9ac4+Wmg5U0AGelI33qO9Dct7UCFXg0Kf";
    os << "vUgoTq1Axe9Rv1NPzyajbrQJiilfvSU1uQaAHZ+YU7OaiB+YU9aBDlqTsKiHU1KPWkMXGfc0/IYgZxT";
    os << "IcEH604HGQe9IYEc88jOKGGHOfWkkxgc96JGBfAoAdn5TQmCaZI22PPfNJHIDjcPyoHcsNhscdKaOTS";
    os << "hlP3TQuT0pAIRTGPFPYcc01RnNADWJODTqcVGOlOx29qLgRY6n0pVbIJp20HPFMXjigB4px/SmqKlA6";
    os << "Z6UikQMMU09akkKqxBIqCRwM7Rn600SxxPytSRDKnH40RndGxJpsRxwaYFsj5BjgetEhABGc8Uz5TAO";
    os << "eh6U+XDBQAenWpKK7DCe9R5wfep5xhAO+agxmmiGTr0zmkU8sDTQePrQ3UUFD1I204e1Rg8U9TxSAO9";
    os << "NPWlJ5ppPNMBT94mlTpTCeTSqcAUCQppV6ikPNKnWgZIetJM2CmKdjkUkq52CkMaxJPWmL/rKkK801R";
    os << "+8FAMVW9alY4AxUGOakPKgdxQMM8UYzx7UqD93nvTguBz1pANwM0idqa5y3tSxcmmIco4NKT0oXpQOu";
    os << "KkYo6UhXg9KU8DNNQ7s5oGJt+8Rx7UFRt5p3OScdqRm+WgYMNoz2pVbdilk5GKijO18dqNxPcdICW4F";
    os << "OQYXBp5HPHpTQMGi4W1Gt96l96RvvUZ4oENl5GaTFPIyPamEYOKaEPjPapZMBDkelQxg5p8pOCKCuhA";
    os << "D8xFLjmmsMMKf3pkgfv1KAKhPLinZIpDQ9B8zUnrSqQSfWjoSPWgCOQcimfxVK/rTCKZLI+uaYKd3NI";
    os << "vWmSKOtOpop34UDQpxTDTj0qNiaSGwDcn1qSNc5zTY1x161IrbUJxmmxIWTaNqnqe/pShdhyvWoH5NS";
    os << "xzDo/OO9Kw+pL5mRgjbx1ppVBHxinoYynLLiqzbB0JI9KENjXG85A4FEnzbcdBTj93A4FIeQABTJI9p";
    os << "A4HFKoIYEg1My4WhGQn7wp3FYj8xlHBNOE55yAak2Kw7GomQKT/AHfX0o0G7i+aMDK8/Wn+cmRw1RmE";
    os << "DGDxSmI9jRoIkM6Z7/lQJkx0aovKOfvUohIHWjQLitKCCAp/OkErY6CnCHI60zYD06D9aNAGSSswOSa";
    os << "FyT0NP2j2FPVkx94UwRXdSDz0p8Z2Hn7pNSOuc1H0wDQA9MLMCeh705FUk5I61GjYbB5HpSxhCSCSBS";
    os << "BDy/zEKNwHeoyC5yevapSUA4YCoJJccL+dCBkisGyvcVE/FRgkcinbt681VhXGtTDTyOKaaBC8cU8c1";
    os << "GOSKlHWgBqj5jTu4pBwaXPSgaBuStLSHqKWgAH3qO9IPvU5euaADrx71Kn3RUK8salXoKBoQ0lB9qBy";
    os << "aQABS9qUdKQnFACUjnApRTWOTTEJThwKTHrSmgB605ulMTpT36UhoYOooPU0DrQ3WmA9fu01qcnSmP0";
    os << "pAIeabSr1ox6UxCpyKU01Tg0rdKAHU1h+VKDSmkMaaUUh4NKPemIc33TUXQ4qR+hqJuCKBsXvQ33qD1";
    os << "pD96gQtNXq1LSDgmgBe5prDmlz1pDyaAFNM45p561GepoEIKcKaOtOHSgCVOalZgu1e5qDfsXjmmkk8";
    os << "mlYZOAUOV6nrUgf5gCNoPeoI5c8N+dWMoQMsDSY0I6qDximOd0xI6CiQICACSKR2y2BwPSgGNkbf0+6";
    os << "KaikngU4c5AqVFxTAhbI7URysuME1MzLj7wpmwexoBimViOgpyygAAqfzpmwDGfun9KeYcDrS0AcZkx";
    os << "0agTpn+L8qYYSR1pvlHP3qLILknnJk8NTfNGDhefrSCI9zSCEHOScUaDHGduMAD8Kb5jMOSaFTJGPu+";
    os << "vrUuxVBzgUaArldsliQKNpxz0qZ2QH7wpVXK0XFYij+Xdn7p/ShBsOSODThwCD1oH3cHkelIZMFQx84";
    os << "p/m4GAAwA61XXYepIx2qy5jCcMoGKTKRHt8xiW60ke0llHUd/WkkmHITv3qFOD70WF1JJVxjFRluR61";
    os << "MzbowcYqORc9OtNCYA04VGpqQUmNAaaaU0hpiY096d0ApG60vcUCH96dGOaQCnp60i0O9KHHzLS5yRi";
    os << "kdgCPWkMVhUQ/1lOOW5NNHD0wYmOaQn5gKf3qMDLGgRbjwUGBzzUcrHkUsJwAKbIDk0iugzFOi4BNNA";
    os << "ycU9Rge1Nki0i/eNGeKF+9UjHOMrgdabGCG5FPNOA5GfTilcdhjNtJpoG7mmSHc+O1Sx8LinsC3Fzmo";
    os << "2XkmpBTQOlADk6EUYpF/iFGeaQDT945pvc0ueTTT1qiWO7UpAJGaTOBSk8igY8YApjmnZ4FMc9KBsjf";
    os << "iQVJ3zUcv3wakY7V5pkjT98GlNIjgkD2pWyOlIENJwSRT81GVOTxxT6YDuq0zpmnr92mmkBAepoFKw5";
    os << "/GjtVEiClB5oFL1oBCN0zSIOppSMnHalHXigBTxntUanOTSyHJwOnem9sUAKOW9qCuCcUo4FBGQeM0A";
    os << "MywGBSLn0qTbhTjnHWkV1z3pgO2kpz0p6qAvt60Fx5RwPzqIsS3r7Ug2FuH3LgdPX1p0CgLvxk1Gw4y";
    os << "acjlOvSmCepZwNmePrUT42vTt6fwg5+lM5bORgelShsVecBvSnc+nNRs+0jHWmPIxPJ/KnYVywF9qVu";
    os << "mKqK59Wp6yHHB/OnYSJuitj0pAoEaemKSOTdnPBoYlF46H9KQxRgLniomwU3YwaXevUg5+lRs5bOOBT";
    os << "SAI3xwen8qkdQVU9c1Fj0pd2MdqZIzBB47Ug3ZqQMM8j8qRnUHvQIZkkn60hBzUmATk9D2pCMdBincC";
    os << "M8UgODT2HFR0AP7U09DQDg0vY0AIOoqQnOKj71IKAEJ4pOhobpQO1ADgeafTFp460DGt96nJSHnP0oH";
    os << "agAT7xqRaZH9409T1oBCGhetDUi0hjqQ9aWm9OaBBSYoHNKaAF25x9KMc07OMUnWmMUdaVzxTaGPNIA";
    os << "XqKVuppB1pW6mmAqHikNCmigBMc8UFcZpc4peuaAI8UA/lQKQ8UCHDrTqbnJzS0AI3WlHSmt1py9BQA";
    os << "rVG/3hT3PSmyfeFAMRqReopTSCgB1MPWnGmGgBO9KDxSHvQtAhQcUw9TTzTO9AMB0FLR2ppOTigAPJp";
    os << "w5NMqRRigAC80ZIP404DPUZpcAHI7dqAGndnpS4JPPelV1J707cMjA/OkBJGoCt2xUcj54A4/nRuJz3";
    os << "puMdaBki4CbsZNTHBXPFVlcrjPIqTevUA5+lJjuSEAxt6Yp38K59KYpLjnp6etLJJtxjk4pDHryMUhX";
    os << "vioWkJHJ/Ko2c+pp2EWufxprDGdvpUKSMDwfzqRX3HnrSsNDkxsSpcDZnioeVxjkelO3rzuBz9KBojn";
    os << "UFdwGDTbd9owenr6UO5fp0pq9Min0F1JmVWHqPWmYIXjpSBiG649qlDjyuRSArtn0oyxGDT2dc96Xbl";
    os << "eePSmIaFyRSnhvagDGOMd6UnINIYjHABFSDnFQ9sU+M4OD+FAA46GlXpmlPXmkAwcdqAAnmkNO6U00g";
    os << "YGgdRQOlKBz+NMCXGcU/otMFPblKRQ3OOlMByQTTs00KfTigB4/KkH3zSqCetI7AEg+lAMXvmo05c1K";
    os << "p3LxUcX3zQBLGc+1POGWooz1p+eDSKEUAZpOxoB60Z4oEN9KePvDFMHWnZ6UCQ/HNDngCgnmkb+EUih";
    os << "qryDT84pCOtKaAIwTnmhTyKQHqO1JTsTckHU0metIDSUDuLSGkJpKYrjz0pCcUD7opp5YDtSAee1Nfn";
    os << "HtSk0hoGI3LD2pHbI5p5GMVG4xTEwAwwx6VKDkVGByMU/GB70AgNJSk8UmeKBC54ozSZpM9aBjDyaT+";
    os << "Gj1o/hpkjQwPFLnjg1GOG5paYiVRxzQx249aYGI5pNxZskYFIAJxQnqabnLe1OHSmBIuCMd6TBBIpg7";
    os << "/Wl3nuaQx+MA5qHad4AOKkLZz1NJnbksaEA4INmSaMKOnpUbHIyaUAqo9TzTEKRubApzgEgDoKD8i47";
    os << "mnxDJB9KQ0SBdoHr3qCRhkhT9aklfHHf1qD3PFCBiHnrT9qry3Wmryd1PHDZIyT0piFBH/PM49aCiso";
    os << "K/kacN394Z9KGycnGCBzSAixgHFKjZADH6UE/xUwjIyORTAkK7h70xRjOe9SRNxjGW9aSQcZ9aAGAbW";
    os << "waVtvGaPvAg9aY3zJjuKYhCoycGmhfnxSLw2al+9gjtTEB5Ge44pCCcc0hPJ7UhcnvSGK2AMd6hanHP";
    os << "emmmITrThyD60zoacKBC0/PFNPQGlFMYHgY70q9KRuvHpSjpSAdnBozSGgHigY7uaTtQDzQBmgQsfU0";
    os << "5eCaanU04UDBulNBpW6U0UASDkVGx5xTlPyUw9aAHg04cUxeTT6AQ4npmmEkHinGmMeaAHA5NK3WmA8";
    os << "4pT1NILjvSl9aaKDQMVetBOKQHkU0nmmIUcmpAetRA808d6BiHmmk08VGetAgU84p54FRjrT2Py0AIT";
    os << "Tl6Uw05elAA3JFJJyRSmmt1FAMB0o7ig0d6BBmk6mg9KQUDEbpQOnvSnpSL1oEHam96dSdAaAEPA96Z";
    os << "Smm9TTEOWp1xjHeoVpwzSGSgEd6XoCe54pgcjvSg80hjCvz4FOCjIyaeflyT3qJuWzTETrjJxSEbmwK";
    os << "avypjuad90ADrSGIw6YqQLtHvSRjjPpSyscYxg0hjWbghT9aTGQM00DAyelPB/ioAeEVVy35CkJGP9W";
    os << "cetOXIwcZJFB3f3hn0pAM2q3K0wcU88tkDBHWmNwd1MB8bDIDc+lTld2R3qr7irET54PWkxojQAEg9D";
    os << "TANrYPSppRgk+tMA3rg9RQAYXv6UhQbODTSCyn1FNU4GRTATad5Gc1PjIGOaZnccqaUPjHUUhC4JIBp";
    os << "WwBjv/KmbyTgGkPagYj+ooHNKeRTejcUxEqnP1pWHHvUW4q2QMinFi3NIBc8cnmkLDpTTSdW4oAl/ho";
    os << "HBo/ho64oGS5pc8UzPSlz1pFCmgUmeKUHigBS2BURGWOfSpMZHvTCOTmgAVsDinLwx96YozipcZzQCG";
    os << "ocZpwPWmilBoGKDmgdOaaOGIp2floEIKXNN+tAoAfnpTj1BqOnE0hgx5NBJzxTaCeg7UWFcaKXvTc4N";
    os << "OpiFBopF7UtAxDTTTqRqBCqeKVRk0idDT1/pSGhmPmIoHoacB85xQvU4oAG+8Ka46U5vvUN92gBq9ac";
    os << "1AGGyMUHgUANPQ0lKehpvWmJijoKRuppyjimuORQAw0dqD1pD93NMQ0gHvSomT14pmSTSk7RgdTTEOb";
    os << "HbpTGPOKYTjvTo/vZPJpANANPXOD608jk46U4L+NAWIgSCM08+9DfdpM5GO4oAbvOSAKaOT83Wgj5iK";
    os << "WNSxpgSRLkknoKeBk5PanYyAq9KSU4wopDI2yzZqZTtXPb+dRj7v1pHbkAdBQAhJZj3PWnbFHU5NKo2";
    os << "Jn+JqWNRzxlvU0ARkcgAECnA/eP4Ur8t1JpiHcSOtAiUYx935fWgtnafTikOcdvpTTgEDGDQMMHBGDi";
    os << "lCqRx1pyYxwxzTW9ejetAiMkoeeDTgdyj3/SmyksnuKbG2CB2NUAAlWp54II70EcUiHIKmgQx12vkdD";
    os << "Tfcdal6MVPTFRspBx2piGlzmjvSd6XocYoACSTTGB71KOnFNfoKAIjmlXpTu1IV4yKBCqeaeBUQOaep";
    os << "zx3FABwTzSjpSMDQDQMdSjpSdqcBQAtIOhp3YU3tQMVTyad2pq9aXtQAh9KO1OpG6UAN6CikNKvWgQ5";
    os << "RTqSloGIaaetOPeikAzNO+tGcmk9aYh3alpO1A6UhifSkpaM4NMQDrTl60nQ0DtQMWmsKd2pKAGUvUU";
    os << "N1pBzQIdjiihelL3oGHamt1FO7U1utAAelFJ2p3Y0ANPSk7UpHFN7UCA9KTjPFBNABoAUjimsefalc4";
    os << "47mmZoARulIBTwvGTS9qYhFBxxTwTnBoXpTjSGNPWgOc0nU9KTvQA/r160qLubnoKRVJPtUnVwo6UAK";
    os << "OSSabyzU5zgBRQBx70hjidqn2/WmglzxzmmyNkn0FOiJVPc0DJCqjryaTBwBg4pVHfq3rSvjH3jmkAB";
    os << "sbj68UHGPu/L600YJIxk9qUZx2+lIBCfun8KaBgkYJFDnBA6U9OD1xQAmxT0ODTQSrDsetSSKOOMN6i";
    os << "kYb0z/EvWgY5juXPb+VRL8rUqNyQehoI+UH0oAceDkd6ZIuDkdDT4jnKmlxgFWoArHg8dadvJOKSRSp";
    os << "pB94CmIlHtTCSScU7OBjvSr92kAxgcCmEGpyvpgUgHPPSgLDFPOKeuOh6VHIfmyODTc5pgTOmD14poA";
    os << "HekB3DB6ikyQaAJe1KKQdM0o60hj15NKelIo5NOYUhjacOgpnSnDoKAQ9aa33qVeRQRlsnFIY1B1py/";
    os << "eNCn5aVfvGgBh9KXHzAUrdRS4+cUwGsO9IelSN3+lRv296AYlKKQdaXvQIdSE0Chu9Awpp5NLSE5OKB";
    os << "CLgn5utO70qgd+tHegYAc0nanL1/Ck7UAJ3oPWiigQoGAaUEUAfJmmnrSGPHXijHyn60qj5aXHFAxjf";
    os << "eoPK0OPm4oP3cUCFximmn01hxQMYe9JinHpR2pk2Cmuead6UhGTQMbTH+7xTjxmmE8U0Sxo9KTa2eRz";
    os << "TkHNTL7jii4iqVPXqKVTlhVl0XaWXpVfG1ge2eaLgKp+cn0PSrGQVDZGe1VAdr1MkiY5z+VDQ0EowpO";
    os << "eKhVvmJqZz5ijjCj9aYBzjGM0IGLtGSxOacD6Co0Bb6ZqcAKMmgEOA2LnvUYG5snvTiS3Pagnb16d6B";
    os << "iSHaOPwpgGRmhzubinbgCFoESYwy/TikU/K315pfvIAPvLUeSWJU4buDSAeOVOOtNGAwxyOg9jR/Fkg";
    os << "g+1M3Abuo560wZKQPxpVAySenekXfjgjHrQSCo6nnOaAEIwnPXPFI33fxpy8lsAk+9NJ24JOW7AUCEI";
    os << "Bc59OaiIwoqUDAOfvGmEgnb2qkA5ORzTWGDxQDtf2pzcgHt2oENPzZPtSZ6A0gyHPpUmA1AEJUZz0ph";
    os << "OGzUjKQfahhyeM0xDlwVBpsgGPejOzn+E013Ujv+VIYwnGPegnAoJyRQBkE0xDQOaVN27IFOC9CelSL";
    os << "7DigSEPOcikwMc0489aQmgYgHanDikooAfTe1KDQvQ0DEXqadSJ1pwoBCZpCcilbpTRQAh6UL1pcfLS";
    os << "L1oESClNIKWgYh5paU9aTvQAhAzxSDqadgZ4pMYJpALRRS9KAG96UAZ5pAMkU7Azz1pgJQBzR3pQOTQ";
    os << "AgpKUUhoAY1A6UHrSj7tAhQeKKaacvIoGLTW604jimt1FAMD0paQ9BQTQAh5pD6UtIeaBCY44pynpgU";
    os << "DpQOOlAET53cim1Oec5HFMZepHSgBAcigc59qCMAGkB2k0ASpjGe9ObAUmmK6gd/wAqXO/2UUhjAcsa";
    os << "cFG7PWnKOQMYpFUk+1MQ7PJApR8uCfSnYCjJqNslx6UhjlG4805+BQvAPp3ppO5vagBAMqakAAcfTim";
    os << "ggHb2p55UYPzCkMVfu/jSgZTjrk5pud2SOG7g05sArwQfakA5gNwx07UgA/GkBAU9RznNDb8dR65oGN";
    os << "OC3PTofrTjwoz1qPcDt6nnrTs5bgEn3oBDmPyr9eKdjLt9OaiyQwLct2A7VKPlQg/eNAIhIwM06M7hz";
    os << "+NJuBJWmodrc0API2tnuKkI3rnvTQd2cdO1AJXkdKBkZJxyKbtGQwOKlYBlyKhcFfpnFAmNZvmyamiG";
    os << "VBpmOemach8tTxlTQxIlyApbIz3qBj849z0pzyIRxn8qiJJehIbYrHDGkCnr0FOxls9s1OqLtDNRcRX";
    os << "2nPyilqw3sOKhcZNCYWFT7vNPqMHing5xQND16071poGDTvWkUNPWlFHUUo6UCFA4pcDikUcU6kUNHC";
    os << "0q/epB0xSoOaBC4+UfWg9adjIpGHymgY0kGmkcCkHWnHlKYhBSd+KWgUAB6UEc0U5uo+lADO9I+Aflp";
    os << "x60MB260AIAOp60o60h5pV60APHf6Uyn9z9KYKQwNNFOb9aTHrTJHfw0hFKegpR1FIY5BhaC1OA4Aps";
    os << "g+YYpFdBknBFIMkU6TqB7UAfKaZPUO4oNKPvD6UpGBQMjbpR2pTSe1AhB0ox1pyjik6UAV275NNzxQ/";
    os << "LGkPOB71ZAoOAPU1KmAe9RhRuA6UpG0470hofkqAScjNIQMn0NEo+QEH8Kb0ApDZGQc59KRFJ5qQNl2";
    os << "xxUiLxxTFYbG3BVqCDnHanFc9qYwweM0AKMLSn5sZ4FIqHPSpFA6mgAA9evpTZjjCjr3qSQhV3d6rn5";
    os << "nyaENiDipFXoQM0bMHJ/KntJyFQc0CQFGxkcVESMneMH1FWTu/vDPpULANu3dR196ENjOQRhsrS4HOf";
    os << "WkHK47il3dwPYimSPCe520cAAe9NXy8Z3fhTt38RGOMAUgEGW3ZYhRSAgD5Bk+tL92Mj1pwwkYwMk0D";
    os << "GIjHlsc0roMdMc9acmTxvANKX6qw5pgVyM+5pynIC/lSleTt/KmHgqRTJHEfMaQfL9Kcp3Env3pcZPF";
    os << "AWI87gaQkg9aV15PHeo2HODmgAdgRgUzHHNPwBSkUxEeKeAMfSkY/MKUfcNAARu6dKkIHGKjjHU/pTw";
    os << "M9uaQ0IeQfamjpSt3ApB0piF7UvakHWnEUDE6E0g6U7ufpSAcUAKnJNOFJH1NKnU0AgbpTAKkboabQD";
    os << "G/w0ijJp4HHNA60AKKWgUUgFI6UnenZ5FIetAw4696RqWkbrQIUCgigdqD3oGIvWlOOvehetLQIb3pw";
    os << "HJpB1p2etMBlIadSGgCMjml/hp3egjigBpFOUcUlPXpQAhpj8EU9+opsnUUANPIpepFGOKB1H0oATtR";
    os << "2p2KYaAA9KUcAe9IelKvbNAhwA5zTANvWnkY+tMkHQ/pSGwIGKZjj6U8/dFIp+Y0xDMelSRtxg0Y4oI";
    os << "BoAcpJNOzgColHPGalReRx3pDBvm+lOVfmFOwAeaRjtIPftQAjHAK/nTANvsaUcliaeF5G78qAFRRjp";
    os << "nnrQ6MOVxUgfoqjmmvkcFhmkUMOCPnGD6045XbhiVNO4eNsjBFNzujAPakAvBBHvQye52/pSbv4gM9i";
    os << "Ka2zGdx+lAgwOCPWk5LHLfLRu7kewpDwuPWmAoI3DZyfWpAjYyeaao2hdvU9KmG7+8M+lJjRCV6kjFR";
    os << "HmrCyckOOaj2Z5FAMWE5yp69qeRx7+lQg7XyOKnjIYBu/ehjREMr05FIcNUjAHkcVGyHPSgQAHOO1Ej";
    os << "cBVpFGTzmpNoHQUAV3UjBpQDn69KmdeOaYWw696LhYeAMj0FLksDg4GaZ1BpYh8hJPfpSBCvgnv+NRE";
    os << "5B9RTwNx96RgNxA5poGNzxT17YNMHpSpwwpiLBGMUp6UvXmlYfLUlje1KvSkx2pQOKAFHFL3P0petIf";
    os << "vH6UhjTkCljPJpSPlFJH1INAhyt1pXGVojHzHNOYcEUFdCEClP3aO5pAODQSNNLSYpw4+tMBKeR0phq";
    os << "TuPpSAjPWg4PI60N1pOlMBR6UDrR05pqnJoBkvc/Sm04Hg59KZSGH1pO9LSgUCFboKD2NDZwKO4oGSD";
    os << "tQ/bNAobGKRRHJzIKXPBpG+8DQxpkiKfnAp7daYv3qeaAQw0nenAZJpQOTQFgAwlMY8VIfu1E3ShAyu";
    os << "eSaAOM0DqaeBwAKogTd6jNMDY7U/GDj0oA5oACSwxjims2NoFThQAc1UdssTQgYZO8kVNE2e/wCBquD";
    os << "ySRUsRXHIpsSLWwge9RnIyTQGXPUikZhzhjUotjiTjNJuA6c00bSMkk/jTlZVHAosIRgzDJpQAp4od8";
    os << "pxTAM9elMTElYtwOacg2sMnkilbhQFx9aVVJUBsZHQigB4wqgEHOaYcqWJPWnP5gxyDTNhJO48egoGy";
    os << "JcqCwHHpTvvHcnX0oYPu4HA7UFcnpg+1MkcCcj92M0Nx80h57Ck+YcbzikK4HQk0AByQXPSpT8yqR2q";
    os << "NN+TwMY9aUqU+6fwNAC46jByaJBubAPIFMy7HGQKft2xkL1PUmgZGjFWwRUjYYLTcdQ2PrTSMbcGmIT";
    os << "BDEinIwJGeDSBsHmlVgTyKBDj1JqNqU7QeCRTGIx940IGAUmkPAxnNGRjnNBK44pgMP3qcDwRSOcngU";
    os << "3PNAh6naemRTi2e2KMc8U8rmgaIyeOBTQMipigIpm3BwaAEA7VIPemkYIp3figBrdTQKCMsacBQA2P7";
    os << "xp601QNxqRR3oBCEelIB1px6UgpDE7U3vTh0prDvTEOFBoFFAx3pSUp6D6U3NAC96VxzSDnmnuOKQDB";
    os << "1FK3U0g6ilPU0ACDminJ0zTTxzQAlO9fpTM08dDTAaKQ0tIe9AhvendqQDvTj0oARhmlUetB604UDGv";
    os << "TH+8Klcd6iYDcKBMDSDqKeRTQORQAtMIzT+/NIBkmgCM8ClB45FOK5OBTggAoAYGx2zTWO4+gFShcUz";
    os << "HJzQA0ngCmj71GeaVDg8igQ4cjrj604gikBFJkY60hj1qTuPrUKkY+8aeNpPJJoBDnYA8cmmYJYE09m";
    os << "UHims2TxQBIuFDVG7FmwKUDO7Jp2OgXFADo12nB6kUmOg7ilC7owG6joRTMupxkGkUSj5VYnvUIyAHH";
    os << "SnYL/eP4Uj78jgYxQIVefmQ89xS5OT+7GaaFyOmDS/OeN5xQAnQ7n6+lNbLDcR+FKFweBk+9Kofccjg";
    os << "9qAJBlipB6U44KkAHOaj2EEbTx6U9PMOeQKRSGONzHB5ApsTFeDxUjLhSF6nqTTV5UhsfWgQpAY80KG";
    os << "UZHNMIxjB4p6PheaAQbgevBpcnGaGKsORTSFHIJH40hjhk4NP2Eg+tRKw4yxpxZc9SaBpkcrY759hUO";
    os << "TvBNSSlcDA61ETyDg1SIZKp+8DTlJUYxxUSNhgSKtlQQKTGiuW9qdu9BSkc0mMtg96AGleM0o608jgg";
    os << "0w9RQBYQ5FSEZSo06VKPu0mWiMjnFAp5GSKQ8MKQWFXrzTWPzkU4U1vvUAxewojOHNCmhfvGgCRO+KQ";
    os << "9DSrjHFIe9IoYO5oXoaO5oHSmSM6Gl9KCO9FMQU/uPpUdPY8DHpSAYepoPpSE4NL15pgI/wB2kSnHkY";
    os << "pqjBoBjzSd6cBkH6U2kAGlHvSUoPagBT0pe4pD0o7ikMetK/Q0gpW6GgojP3hmg9aGHzAGhqZIf8tKU";
    os << "01fvCnNQCBOrUozk00cE04Hk0DGP1pr/dNStjbmom6UCZCvU0q53UwfeNSA9DVEDP4uelSjHGKj4J46";
    os << "GgHmgELOflwSQPaq3HPJq9wwOeneqTrhiKEDEXHfNTpjHHSoAD0zUsS5BBamxIlOcDgYoI+XpS7ACMk";
    os << "01gvPJpFCj7pwO1NUDHNKu3GN2KcEBXjBoAjO3b1/WjjHWnuhC8U0H1oENbHHNOQjPU/nSuOAyjNPRh";
    os << "tDMAPSgERueB8x/Omg8H5j1qZyxC4SmhxkhhigGR56cmm5Oepp7MwbAAoLYPzEZpiIyT6t+VLk8/M1P";
    os << "3g84NLuyPlI/GgBmeepoY/L1NOQsWIwKczbuFGaBWIFPI5NS5GOp/Ok5Xll4qQkFNy84pjRE2N3X9aD";
    os << "jC808c5JGKaT0wM0CGnGev60igZpwUk80qIO+KAAgYpmBjinMF6Z70xgo6E0AxR34o9KaAMfepduO/N";
    os << "MAeo2xnqac+QeuaaRk0CHr0GCalFM6EY6U8kCkNCikPUUFgBTd2SKAHnqKKaTkil70DD+Kl700nDGnA";
    os << "0xCdzUidBUQPzGpF9KQ0I1JTm6U0UAFBoHSmk9qBCrSmgUUwFprdaeegx6UwikNjo+lPb7tMXinv0oA";
    os << "YOooPU0DqKD1NMB6fdpsnSnJ0pjc0gGr1p+aYBTx0OfSmCEBprdKWg0CAdKWmqe1KaQC0q0hpV6UDHP";
    os << "0NRHqKe3pUZPzCgGPzzSfxUE00HLCmIfSDqaTqfakBwTSGKOppSaZuwaeGBFAhGqNuh5NSAg0zrnPSm";
    os << "DI1xmnpj3qMDBpyZPegRJ60h+lJtz35oIGOtAx3GOacMVGoUjkmnqFzjNIEI2M0DGev6050HakKkHig";
    os << "A4w3NKpG7r+tAPXIpx4wQM/jQAmeOp/OomPJ5NWAQE3NxmozluVXigbEU/L1NBPPBNSK23hhimuWDAY";
    os << "FIQzceOWoyfVvyqTdgfMR+FJuA7GgYzcc9TTs+5pQ24/KRmhWYtggUABPA+Y9aVD1+Y04uMgKM05CwB";
    os << "ylIaI3Iz1P501cc81MzDaWUAjvTEHBZhigBOMdaBt29f1oJ9KciErzQAjAY4px+6MigoAvOBTW24xuz";
    os << "QMcANvSkGeeOKRQpxyadtBzhjQAx8Y56VAxHYmpJVwAA1REHpmmiWLxx1qzAfl4JI96rouWAq5woGOl";
    os << "JjiNOOc1Geox0oJ55o4B56CgGKx+akfqKcT1NRnqKALC9BT0pi9KlXG2kWgJORih+q0E4IpDyRSAWm/";
    os << "wDLSlFNb71AMcKQfeOKFoX7xFAEqH5RSMaF6UGkUM70DpSdzQOlMkD7U0UpPakoEB604U2nHgDHpQAx";
    os << "6VD8vFNYZNOHAxTBBQKTpSr1oAf3P0ptO7n6UwUhhR3oNJ1oEPY8Cj0pD0FL3FAx46ChqVT0NNkPzDB";
    os << "pFdBsnDig8g0SfeB9qAeDTJ6iL94HtTm4NIPvD6UpOeaAQ00A4NBpPegQ8HKVG3Q05T8uKTrQMrHgmg";
    os << "HjFI/DEUh4x9aozHhTjkgU0Bj3pwI3DNKeTkce1AwO5RzjHrTHXkEU+UjYB3pOoGKQMhx85xzUsS+vN";
    os << "IFxIe9Sowx602CJd3HAAphJOR0pN3vUbN83BpWKbJCDjkZoC4+6cGmhzmnrg8dDQIaznGGowG6U+Vdy";
    os << "8dRUGSrelNaiYsgKcr0pysGZeOgoLbuDTmjKsGT8qAH5LKDu+lMPzbhjp1qQ+yHNRE7d5JwTSGyIEkb";
    os << "R19adgKcAbmoUbVJpcEdOp61RI4eZxyPpmmsA3DDa1ACdNx3euaeQTwfvDoaQEeSAUPX1qXARVx1NNI";
    os << "DJu7inEbo1IPI5oGNz1OelDkIc9iOlKmMZKk5pShJLN+VAEChnbnpUrAKFNNLYPy9aYxyV71RIbjuIW";
    os << "nKvILcmlVdpPrS8A0hidMjpTGzSu3J+tRM3PJpoTY7PByM008jjijOe9LkYpgRt97BpwGcmkYfMKcp+";
    os << "Q0CEUEtx+dOYEck0kZHIPWnjjrg+1A0MYHGcg0g4+lObgnFNHSgBwPQ08VGKkJoYDW6mlFJ3/CgGgBU";
    os << "HzmnrTY+ppydTQCFJpAc0N3puaQx3am96A3HNA60CHCigUGmMd6fSkpccimnrSAXvmlc0lI3WgBR1FB";
    os << "6mgdqD3oAVDzSd80i9aWgApfX6Ug60uOTQA2g0UGmA3vTu1NPWgnjigQ4nGKUGmZpy9qABqY4+YU9+o";
    os << "psnUUAxDQvUUGk7j6UAONNJ6mnA0w9aAEJzSgHGcgUh6Uq8kZoAACehprAg8/nUh56YHtTJCOAOtAAR";
    os << "jBpq/e4pxPyCkUfMaBDhgDnmlJ44GKTNGcd6Bj1zT+uBUCtzwalRuR9aTBMVl5JXg03cdwDVJwTSMu4";
    os << "j1oGOUBgxqJgyNx0oU4J7VIGyfm60CFQhz9B0oz3z1pwQghl/KkfHUKRikULjerZ6ioskgKOvrUo+WN";
    os << "iTyeaaAFTd3NADVAXhRuanHzOeR9M0oUjgfePJNNITpuO71zSENwGOCNrU0kgbT1p+CevUdKRhuUGmB";
    os << "IPl2jHXpT8lVJ3fWowd2wg5I7VKPdDmpKRCxCs3HUU2MF+TUixlmLPTQ23gUxC4C9aVXOMLUWSzetWI";
    os << "l2rz1NDGhhXP3jk0AHHAxStgcdTTGc5pAPDEYHWnluOQDVdW+bk1Ju96Ghpkcq+nFQ/xgVZdhj0qIrm";
    os << "QdqaJYIvJJp/zMOOnrSdAadERsI70AiMgjvTypxwQaBwcnB9qQkbjigBCeMUDkim9c/WlTlgKYi0g4F";
    os << "PJwlMzilY/LioNAJyaBSZ70ooEPXrTG++T2p445pp+8fpQNgOAKI+ZDSk/KKSP7xPtQHUkTvSHoaIz8";
    os << "xyaVj1NIroRjvQp+U0dzSA8GmSJ3opOlKv60CFp3cfSmGn9x9KBjDQfWhutJ1piAYP1pR1oXB5o70gH";
    os << "jv8ASmU5etN7UABpopaOnSmIdjK5NITSj7vvTT1pDJUOVpGHShThaUnigYyT7wpASBSv97ig9KBCnqP";
    os << "pSGlApGPFAxD0o7UHkUnamIUdKTPWikJwaAIW5zxTccVKRnNMI45pkDAOBjtUqAZ60xME1MmfYUMaG/";
    os << "eAXHGaTgHHYU9ioQqDVctlto9aSATcQT70I5HFIAWfipEhBGSxFMQsa7ss3SlJwenFDAxqATkHoabk5";
    os << "5xgUDY7AY8fmKUnGM/gajjYr2yM1Nwy4oAUHOc9aZMOjDvTsEcGgjd7A9aBkPXmpVY8YOajYbW46U7b";
    os << "lgR0oEiUysB05qFhuJ3nA9Km+6obuegqI8MeNzGhAxMgkAZxS5689TinHORkgewqMrnOSevpQJkoZc4";
    os << "wMfSg84PocUKTt+78vSgqAo54z6UDEUhdynIBpAMD5DkelPXPzYYH2NMxnoNrUCFjkPcYpXf5TkjrTe";
    os << "GBOOR1phXDZ7UxiZxTlGMNTcbn9qe3AA7DpmmSIx+Y+tIDuPFNGS5qQYUCgEMwBnNNYZI44od8nHbNK";
    os << "xOTQBG64GR3pucjmpSN3A6DvTGjAHU0CG5zj2qQEYPvUR4IpQcKR+VMB+dnGO9SEDjGDUIIOAakHYcU";
    os << "AB4Bpo6U48daTt0oAOlLnikooAXqTSdqcBSL0NACx8E04U1OppwoGgbpTQaVjxTBQDF/hoVuaM/LSL1";
    os << "oESClpBS0hik9KTvSnrSd6AF4x70jdaOM8UhOTQA4GgmkooGC9aXjHvTQcEUvGeaBB3pQetJ3pR1oAS";
    os << "kNLSGmAxjzxS/w009ad/DQICacvSozT1PFA0KabJ1FONNfqKAY09Kd3FI3QUpFAhM8UnUUUUAIelOHI";
    os << "FJ+FKBnpQA4Ac5IFR53cU4+nFMJAyBQDHHGPpTM4z70E5UD86QZJ4oAM8cU5F3DJ7ULGCOpp4G3g9D3";
    os << "oAFGD04p2M4pFJyM0iNg47ZpDHk4PNOB+YetBwwNRnIcUAx7jOWpuc08cgjsetMxtf2oAmR/lGCOtJJ";
    os << "IewzUYXJz2p/CgHHJ6UigIyPnOPalYhtoGcCkxjqNzfyp7Z+XLAewpCEHGT74oZlzjHH0oCgqeeM+lD";
    os << "E7fu/L0oAaT056HFNyASDnFAXGME9fSnjOeCD7GmCGKNpGw59qmErY6c1EBlhgbWFS/eUt3HUUmNDGY";
    os << "85OKi6c0/bhiT0pqjc3PSgGPhHVj2p5OMY60gG0eoHSjBPFAxoOc4/E0mAp/rT+FWoXYntxmgQ8HJ6c";
    os << "UjrtwV6Gkyc8YwRTlBkUgHgdTQCIncnil3Eke1PeEAZDE1Ech8GmJk/BOOxpfugrjjNRBsNtPbpU6lS";
    os << "gUmpY0xjhQetRkcHPep3B6YBqF8CmgYmOKcvGOKAOKeBjFMCQnpSnpTQcmlqShe1C9KTtSjgUAOHNHc";
    os << "/SkU8U4j1pDGEkgUsecmgdM0qH5jmgQqjrSucLRnikY5U0DGg5oxhcimjrTj9ygSGGnA0nXrRTEKaee";
    os << "30pnanN1/CkMaetBwPrR3obA5pgMQfN8x5p3em4yadQIUHnNHakXtS0AJ3oPWikagBw6cUoFNXoaeKT";
    os << "GgHWjt+NJnDGhTgkUDBvvUH7vNIT81K33aBC5pppR1wBQfagBhoBoPQ0n1piHU1xzTlPFNfqKBiUx/u";
    os << "0pPNIfu4pksYPUHFJvYnmk6GlZcjIoEIX7ChRhqYeafHknmmA5B87AdSanCgLjvURODgU8HOKQ0NmIK";
    os << "Y61AoOcZqZvun060gGPm9aBMMqTjuKUAjpUROXJp0T4PPSnYLlnO9femKcHntwadnbgr0pso5DCpRQk";
    os << "oyMjtSKeMUufl+lI64II6GmIlzll9hTV+6x96QHcgI+8tOjOQSDk9waQCAbVJ/ipABv4+v1obAbuKYg";
    os << "wfpTBk3uDz6UAAkg8g8/SmHp05oYcg569QKAFY5TcetI33QR2pV247k0j9OfwFAhCQHOT1HNRscqOaW";
    os << "QFUJPU01FyRnoOtUgHx9PrSO2en4UpPFIg4JNAg+7n1xSYJOaU8uSegFRu+T7UCAkA470w5Jxmk7048";
    os << "nIpgSIMKAKSTGPekHTrTX6CkA1skj2pCOKXtSFsLjvTAaDzinRs27ApAMU9BjJoEPOe/akyMU1iaTGa";
    os << "BjwaXrzTe1PWgB1N7Uvak7UAC9TTqavWndqQIDSEYFO70jdKBjD0oTk0HtSr1piHilPSkpaBiE4paQ9";
    os << "6KQAcBuKQdTSY+aj1oEPopvalHSgYh6ilGCeab6UuOaBDqAc0lA7UDFpDS0lMCNutA6UrdaQUCHAZFK";
    os << "KF+7S96BiU1uopx6UjdaAE7U6m9qd2NADelNNONN7UCDPHWlGe3emYxSqxoAa7NuwaaTzinuM80wjNA";
    os << "hwHFC5BPvSBsrjuKXqKBkseMe9K4ypBpidDTj060ARjIbGaeCCcd6bznJpO9AE2CDml+9j1xTFbB9jT";
    os << "wNrgjoRSAVGwefxok5H0pHHAYUoPFAxqn5SM1ICC4weg4qJ1wTjoelOjBZQR1FDAlX7pJ70KcJnvQvT";
    os << "j8RQ23HcGpGOIAIA7c/WjPcnn0pijknPToKUdOnNAxCBv5+v0pxG5Qf4qicZP19KeuCe5xQCFb7qn3p";
    os << "2cM3uOaSQ4AJ4PYCmk7UJP3mpANY8Yp0QwMnvTUXJOegpSfl+tMBWOTx34FPzsT3pkQ5LGnZ3ZJ6UDI";
    os << "yCetNyoIHc02R8nimj74NOxNwYHOKnhIC46UwjPzelKv3R6UAtCUqCuO9QOPnAPXNSk4zTAcnBFJDZG";
    os << "wy1AfsRSyZB4pg4piJN7A8UH1JzSAYGTR1NAEifdp/GaYPu4pQeaQ0PQc06mp1NOY8UihCaBSH2pR0F";
    os << "Ahwp2RSDpzQeuCKQwH3aF+9Qv3aFPzUALn5aD1pGPIFKTlxQMQikPTmnN/SmP0FAmA60d6RaWmIXtQT";
    os << "zRSN3oAO9Ncc/KeadTcYNACrS96ADz6UUAKKKUUlIYhpppxpKYhV6Uqn1oH3ab0f60hjs/MaQE0EUE0";
    os << "wHN96muelKTnFRueaSBj16041GDgin5yM0AI3Q0lKRxQRxTExB0FI3WnYppGc0AMNJj5aUjrR0WmIYc";
    os << "DrTkYA9Kj6tzSj0oEOIGeOlMYHOR6U7G4YoAYNhqAIgxp6k4JowA3NOHTigBvLYB4FPNIO9GCeeaAGF";
    os << "GyTSL15qbBGeaTaG4xg07gLE3JU9KcDzg1EeB707JKgjqOKQwOVbFSgblx+RpjfMMipIjg49aGCIeVY";
    os << "np2p4KnkcNTpU/i/OoMZJBo3AezHdycilxncDTF/u08ZLcYBH60xEmDjqNtN2Y24780DH9w5pWJHXqR";
    os << "x7UhjQSAcHij5R82cmmnONtMzjgfnTsIV8v1pyjao/WliBxuH4CiQ8YoAi5Y049lFA+UEnrTWOFz3NM";
    os << "QkjZbA6Ypp/X0pFyW96lAC44yTTEQlTnmlHWnkc9aaVI55oATkGmO1OJJprUAMJNKvTNIeTThTEOXk+";
    os << "1P7U3oBSikMTjPNA5FKwAPFKBxQAdqUdKQ0oHFAIdSDoaB1oB60AC9TT+1NT7xpwpDQ00E8UrdKb1pg";
    os << "FApyj5DTD1oEPU5p1MXrT6BoQ009aeRzTGHNIBKWlA556UpHNABR2oFKaAG0lOA5pCOeOlMBB1pwpqj";
    os << "mngc0AFNY4p1MbrQAhopB1p7D5KBCdBS02nL0oGL2pjdRTzTX+8KAEPQUtIxoPWgQh6UlKRxSUAxDxS";
    os << "cZ4pxHFIoGaAFNMYYNONJ1BoAjb1oBNONMHBoESK1O5J5pgp4JoAO9IFOeKcFJ7GnAc8GgY0frTo2w2";
    os << "D0xTiA2eMEVE3DAd6AJx3U0zlTSg5Xd3FOPzAEdaQxWG5T+lNTKdKeh4xSyg43H8RSGB2n5s4NBJwMn";
    os << "iogc8H86eAcbaLAP2Z3Z7c04g46jZSKSen3gOfekOP7hz+lIY3GNoFIrHdwcClOQ3OCT+lMb+7TESHa";
    os << "OTy1N5Zgevam4AIAqaFD978qWw0BG1cfmaiGWbFTTHJx6VGvyjJoQCk84FNlbkKOlJkhST1PFNHI96E";
    os << "gGN14pQpyDUmAvGMmlwTjmncVhBTORnuKfgjnnFIe31pANYnANNLGpD05puAW4pgCg5yaeoBPPSkIYt";
    os << "haXBUYpAK7AnpTRg9KQ+lHRuKYEmOKBR1WgDpSGPXrSnoaQDGKdikMSlHQUAcUAcUAhwprdaXOBmmE5";
    os << "JoGPQ9aVfvGokPNSg4zSYIaSaUn5hSA0AUwFY+lI3Sk6v9Kcfu0gGCnCkpRTELQRzRSkUhje9IaWgg8";
    os << "elMQ4DtQBzT6Qdqm5VgA5NNxTl6saMc0XCwyg/pS96aetMQ49KQjNL2pSORQAhHSmtxipMcCmyDpQMY";
    os << "Thh701lwOaWUncKkcblFMRED8wx6VIBgU1EAIPtTmPpQCA9KO1N3HNLj1oEL2oxTl+7TTSGQnij+HFD";
    os << "Hk/WjPFUSNVMHOaXAxkUo5NLQFgUjHvSOM49aDwfY0vQ0ARkZpE6Yp7jac9qbjuKAJFAxk/hSE5JpBy";
    os << "PajOAcY6YoGP3bgc/nUG/5x1qTcCp29+vtTVjGfvGhCHhlKcjk+tGFH3fSnGP90cYNRYIbHQ+lAC52t";
    os << "7U9jtIweDUbHK88GlVS/0piLO7cAfzqvIgUnH/AOqptmOjHNM5BO4DB70kUyEnByfzqQlW6nDUOmT8t";
    os << "RupB5BFPclkwDf3xiglVHBy1QAE/wARp6oSOMmiwBnqR19aEUEc/wD66kjjxnd19KHywwo4Hei4CZ28";
    os << "96jB3ZzTvLz1Y5qMqVznpQgHZDt7UrAHGaZnHTrSkE470xDSVVjgc00P8+e1SBOewprIM9aBDuAPc80";
    os << "FvrSZAIDdR3pGP0oGIwGMioWqVjxUdMQ0DFPAwD60mM5pe1Agp3am96eORQMaeRmlXpSN0paAHY5paR";
    os << "SaeKBoTuaTtSnqfpQOwoEEfU05eSaSP7xpyDk0DQjDimgU9qRRzSAUcCo2HOalxTSM8UwGgcU8c00Ut";
    os << "IEPI6VGetSHJxSdKBiAc0rdaWkbrQAelHrQOtB60AC9aQjmlUc0tAEY61IB1pOtKMjNADTxTCOKd9aQ";
    os << "80CGqOc1IeRSAY4p2KYEZFOXpQw5pVoARuCKbJ1FPccimyfeFAMb2pe4oPegdvpQAlFONMbNACN0pF4";
    os << "GaD3oXpQIXtTKkNR96YARkAjrTSM0/sKaRg0hAtTKBjJqGpFPFA0SBvrRwR7imqaXIyQvU96Qxhf589";
    os << "qcCrMMjmhUGetOKc8YNMQ5QBnFJkI3tSAEZxxSE569aQxxO3GKkzu571CFLYxwKk8vHRjmgYjqAOP/A";
    os << "NVGehPX1qRMqMMODRJHnG3r6UgAFWBydrUEP/fGKjZCByCKYwI7miwEoKr0OWqPPOR+dCKSeATUiJg/";
    os << "NQCEjQMRn/8AXVjdtBP5VFySNoGB3p+zPVjmkykRqQxOegpmdze1DKU+lIpwOOaZI/Cn73pQWUIMDke";
    os << "lMwS2Op9KlEf7oZwKQ9yDf85OKn3bQMfnUbRjPWnbgFG7t096GAoOCOvNDAYyPxpucgZx0xQeB7UDGv";
    os << "0xQBijHc05BuOe1MQqDGc9aViMe9L3po5PsKQCYGMmkKZOc0+kNAB/DQOaO1CnkfWgCbFHagU5vu1JQ";
    os << "3tQOlJj0pAxzTAcVyOKjJ+Y59KlU+vSmugLE+1AMYq5HFOHLH2p6DYpqOI/MaAHLzmnAdaIx196djg0";
    os << "hjAMUo6c0oHJpMcUCE/lS00dadQAYpxHIpSOaG6qaVx2GMOTQQelOPenUXCwmOKYTgmnyHAzUSfM/tQ";
    os << "hslToTSk0vA6dMU0cmkMafvGm96c33qTFUQwpSemaQ9MU0nJzQMm4Ipj9eKSNuadJ0J7UD6EL8yCpPa";
    os << "omPzLipO4pkCH74FLTT9+ndaQ0MIznFPxTkXBPrSkZJNA7CDhTTOtOfpimZxTERH7xo7UdzSLyaZIo4";
    os << "pRyaQU7nNIaEbG3FIh6048io2yORQhMlYZBzUajjBpyMDTwu5DjrT2AiBwcUMQSe9DcVJHCW5PAoAhy";
    os << "2BgUKT7VcSKLbyO3XNV2CcBWoTCwbyE56U8bSvqKYcheenrSEbQCMY9KAEuEKD2qSA7k2g/MKHIZeeQ";
    os << "aSNEB7/nRfQOpPgbOmKif7r07IWoXcMSP4e9JDbJVPQnincn6+lQmVTilMo9KdhEoPbilPTiq/mjP3a";
    os << "eJgR0osK48nhiPSkBHlrjpikWVQOaj3AcZ47UDJdoZemT61EwwmCfmoBH1+lOWNSMnOfrTDchRCT7dz";
    os << "UzEKqgcUMQvTgVGfmwT0piGFiTxTQzZ7VKi7n46U6MJuO5u9Akivzkk+tGeeOKtmOPHQ/nVeSIjleRR";
    os << "cLEZ5po68U8dMClCbQd1MQ3HFJ2NOOMUw0MBR1FPIximDqKlHJoBDTnFIKXqaXGMUDAdafTTwRS0AI3";
    os << "WlSk6tzSr1xQAJ941ItRrwxqROgoBCGhRzQ1A60hi0nenCkIzQA00macOlNPBoEPDYxSZ5pp569qU0A";
    os << "PHWlccU1Oae/IoGMXqKG6mhetDdaYDkHFIetOTgUx+KQCZ5pS2c00Ug46d6Yhc0Ug5NOPSkAd6WgDFK";
    os << "aBjWHNAoPWhaAFamP8AeFPfoajblhTEwekXrTm64pvRuKAHUw9adSDkmgBhpR0pcZzR0NAABnNRnqal";
    os << "6Goj1NAmKOgpe1NFOHSgBh6804cU8puHy009MGgA3ZPNLzkY9afHETy3AqcRx46H86Vx2KpZs9qcGIP";
    os << "NSyBNw2tSOu1+aLhYepDKwPNROhB9u1KPlyR0qRSG69DQMaoymAfmqXaFXpj3pjRqBkZz9aaSPp9aQy";
    os << "Un92wPTFOz8qk+lQ7geM8d6e0qkDH0pCJF6c0hPbiozMAOlN80Z+7RYLk3I/wprHqRzTBKPSkEqjNFh";
    os << "j0+6lS4Gzpn1+tVkcKQP4e1TEhqGNDJzhME/MajgQsPanyIhPfP1pVIVeOlHQXUU7QvoKZvJTjpSAbg";
    os << "ScY9KUcrx09aAI2J9qMnHIqRQnRmqy8UW3gduuaGwSKikAjtQTk4qSSEryORUa80ADdMCpAMAYpWXag";
    os << "z1pjsBRuAj9qVR8tMXJ5NSDpQwQh60hp3OaaaBsO1A+8KG60dxQST9MU48qKjzmnoeMUi0JimgYxmpc";
    os << "YINI65IpDsNpB98il6U0ffoAf7VGnEhp/c1Gp+Zs0xEye9P4ApsXIHpSSNzSKFGDnFN6g00HBzTweMU";
    os << "CG96cPvCjHFC/eoBDwaRxwKDxTuD16YqSiNTk+9PxmoX+V/apYzkZpsSGsdwwKVF201VO2nZOQPagBH";
    os << "YhuPSlQ5Gaa4zg05RgYo6BrcRvvUdjQfvZoPSgQ2TgUlEvX8KRRlvaqESRjPNSyDKH8KYTgcUxz8nvS";
    os << "K6EYHzZpc804DgmmZyaZIp+/Uwx1FQk/vKcG54pDQ9T8zUmetJGwO40Z5NADZDyKZ/FSv1pBTJGjjNM";
    os << "FSMMA01etMkB1p1IBzTsUikB65phqRhxTCOKAYwDBJqWJsA5pq9TSEcUxEsiqSrHAA6+9KX3EBeTVbd";
    os << "nvU9vwpI65pWGmSeW2Mv932prKnl8AU+PLKTuwMVHtXyT2INIZCxKZXPB6UP8m0dj0psowc9frTpRmJ";
    os << "cVZIzecYGKVWJIGaIk/vHAqxGiK4OM/WjQEiAgsOMmlELnPFW+BwOlRsaVwsQ+ScAkgU7yOR8/6UMcd";
    os << "6k3fd5FF2AwwD++fyoEAx9/8ASpc00Nx1ouwsRNEQCdwNNEbEfdqRjnp0p69KLhYqOpXOQQacrMO9Wn";
    os << "y3BpDGp68H2p3FYqOxJwaco3kDsOtOki67TmliG2PB65pgKo3SBc4ApyBOhApsa5kz0oQKQc5JzSAXY";
    os << "QTs+7703dtJDdaccjByTTZsHb6g0ACIF3Occ9BUcnNPJ4xmoj94U0JjG6001I4phHJpiF9KeOKYOcU7";
    os << "PFAAp+Y0vcU1etSdxQMa33lp1D9VpSKQxBy1Hegff46Ud6Yhehz71Kv3RUZGVFOQjbSGgNJihjgZpBy";
    os << "aAFU9qWmilU5BFAgFI4yKU9RQaAGU7qKSjkZxQBIlObpUan1px6UxiDqKD1NA6ig9TQA9elNegdKax9";
    os << "KAE6Cm0vJxmigQqDApTQKB1NAC0jHtQxwAKQ0gFpRTTwaVTkZoGPboai6nPvT3Py+9NAwppgxO9IfvU";
    os << "d6D97mgQUi9WpwFInVqBidzSE/MKd3NRt1oExx5NMx1p9MPegQ0U5aQDkU9RQA+PipHQNhxjjqKiHU1";
    os << "KDxjNJjQbtxAHWn7CSN/3famw4Ab1JpwycnOKQwcJ0AGKaw2yFc5BocLgYyDmiRcSZoAY42EjselMRi";
    os << "DgVLKN0eB1zSRxdNxxTAazMe9NRS2MAmrYjUdOT70qfLwKVwsVzGwH3aesRIBLAVI3SmqcDnpRcBDAM";
    os << "ff/SgW4z98/lTy3HWnZ5pXY7EPkcn5/wBKb5JwTkGpt33uRUanPei7AYYX44pApA5yKsKal4PB6UXBI";
    os << "osxBxmjccYNWJERnJAx9KglTrtORTuDQqfPkdh1oU78L2HWiIYibNJEMnPT6UAWFVPL5Ap3ltjKfd96";
    os << "ZtXyR3JNSSZUKd2RipKQ0PtJDcH0psaqCzDBB6e1JccqCeuag3Ed6LCuTSnIGKiIyQacBxSt1FMQgp/";
    os << "vTQOKeo4pDQ0009afimkc0wGGnnnFIw5pyjIFBIv8VPjPJphpU60iiX0pWPzLTc8iiQgbDSKHtjvUQ/";
    os << "1lKW5poP7ygGGeaQj5s0ZwaeR8oNMRPGMIPxqKQYyaSM/J70/ORz1pFdCKli5GKa4w3tSxdfwpkj+1C";
    os << "/epR0yaQfezUjFc4GaRGLNSsMjFIgxk0dB63Fdd1Ip2jBpcnJHtTWU7aAF3cdc0m77vY0oXahBFRScE";
    os << "UDZMDwKU4FQLnbntT92Ac0gHAZzSE9KdjA4ppzxTQmI/elGM0jUhpkjs5pO1C09VyKBoFHynNQsuOnS";
    os << "pz3ph6igGQsf3lSLyfrSkDeeKVTg8UxDYhy+aXvQvek70gGP1pB7Up9aQUxCydDSfxAU5uTigjnNACd";
    os << "+aUdfajHTmlXmgBT05prD5aceQaQjK80AMIwQRSt0pxHA9qRjnFAFfGDmpVOE4NMKkE5H405VJHt61R";
    os << "KLEQAUcnkfnQMGMjotEZwtNJyh9Kksgl+ZR7UvO1aR+acPuD61RILjOKepx1plOQE0holFNbFIGwMUj";
    os << "dMikAjDIAo6YFLn7uaawOaYh3frTBk5ApQCaVBzTARRxTxTV6Gl7mkArHningbqYMcUAEMeOtMBrjni";
    os << "j+FaUj0oOdoxQwGqcOPc0D07ZzQByKQcE4FADiATxnHamnnrTifTimY546UCEHQ0xhUhHFRvwKaAQjN";
    os << "NxljTv4aTHNMQIOlIaE4NDUACYyKk71EpwalHNAIHH3aVuvNKw6U1up+lIYi/eFLikT7w+tOxyaAD0p";
    os << "elAHIFO24HIoAawzTejU9v6Uz+KgAB9aRaUc0qgAe9AhTRQaQUADetIOaceaTbzxQAo96XoKQqSvvS9";
    os << "qYxF6ihuCaF60N1oAXqKQ+1L2pApC+9ADTSr60beeaUcUCCgUhpRSAY3WlJpWAI96Q8UwDq1OUYpp+9";
    os << "T1/pQMTrSetSbcjgUwjkikA3FI33qdjkU1/vGmA5etIg+9QvUfSnKOtIBveo2xk1IeKiY5NNCYo6Urj";
    os << "rSLQ/JoACPmFOxikxzS/w0AKtSHpUacipMcUhijjpTgADzk+tMxzzTwfXmkAH07ZzQxy59jTW5IyKUj";
    os << "k0DF/hNCDnmlGdpzQB60ICQjbTFJBOaCCWHHSg45oADTWFL3FI3QUANORgGn/jSOOaQgg0AKecihQQC";
    os << "KRRzTs/eoAcKcajXpk0pbIxSGhGOelMbGcU5wRTTQDE52tRF8qn3pT9w/WmpxTEWTgRgdVolAKnk8Di";
    os << "mg4UccU6U5WpKIGOU5NRYyakZSB7etNCkkYH41RBKvSkAySTTlOAaUDg+9SUIo+WnDpxSAYXilHAFAD";
    os << "T19qTvxTm4pMdeaAG/xEUsfQUoHOaF4OKAGn3pU60jUo9aYiQ9aSYZKYo705u1Ioa3B+lRqf3lTMcnm";
    os << "kUDeOKAGKuevSpnHyjFNHU08dqQ0R9qXOPypzLgUxu9AMU4J5pE7ZpBSrQJCg8GlIxikHenYz1pMpCj";
    os << "FITwaZnIGKY2cZ7UASbvvdzS7vl64qOPnNSFdyAAc0DQpweQTg0x+Rj2oX7tIeppAxOen4UEdetDdV+";
    os << "tA70CJA2V60054pE+5Snt9aaExGGTxQxoPemv/DVCY9Bn8qmB+X0FRx9B9KVvu0ikGcZpufmFJQe1BL";
    os << "EP3j9aUcNmhvvGjvTAF/ipD1NKvU0h70g6DT0po5NL2agfe/CmSOPDGl7g00dfxpf4qBh2FC9RQPvGn";
    os << "CgEHWjjBHrSjtSHt9aQwOB1prYCjFK3b60H+GmIjH6U/nBI/KmjofrT160APXheahJyhFO7H6Uz+CmA";
    os << "w09BmOmHpU0f3R9KGJDSME+lCcd6c3Q01eooGB65FBGBigdfxNK33vwoENB6U5uV96RfvClbrTAEGBS";
    os << "nrQtI33j9KAEXpR3J9qUdKT1oAOpHNScdM0wU7u1AIaOppM8CnNUb9FoY0BPIpw71D3FSR0hIfxnFN7";
    os << "E0dz9aH6iqARutRucdakP8VRt0oENyKUDk0g608daBEY9KaeTT/wCOmimAKMZqRB0qPvUi9qGIe38Oa";
    os << "Rx81OftSN96pGNXqKUn5jQP60nemAvUjtUmRtHOai9KcPu0DEPemDlqc3f6UxfvUASKMUetC9aU9TQI";
    os << "KbS0npQMcaKRulKtAC8Bc0nbNO/5Z03+EUAxFPz0MfnpB94UH7xpgO7ZpeCuaT+E07/lnSAbRQ1C0AJ";
    os << "S031paBB6UMM0vcUN1oGRnhqeO1Mb71OXt9KAJcjaecVH0J70p+7SetAAD8wobqaB1oPX8aBAg+alX+";
    os << "LFC/epU70ARuOtRsM4qRu9R96aAAMU40007+OgBccikyKeetMPWkA5DmpF61GvQ1IP4aBi9gadxnFNX";
    os << "qaO4+tA0KaaDyRRJUfc0gJs8GlPUUxejVItCGx3HTNR9Ceaf3Wm/wD16BMO4+lDdKT0pT0oAUdaRxkU";
    os << "L94fSlagBF+770hPWlXrSN940AAGRigdcmlX734Uh60gB+e9AGT7UN1NOXoKBjX4j/GoxU0n3T9KhXp";
    os << "TQmSA4QCpm5XioP4Kk7D6Uhic4BP5U09fant1ph6D60AOXBU5604YPSkH8VCdT9aQC8YA9KOlA7/WlP";
    os << "egYxupo7GnGmn7y0ALjkmk6sKP4qQ9fxoENPFOHSkP3vwoHRaYh46ilb+GkHalbqKRSA8tmkH3h9aU9";
    os << "TQv3hQAvVjTs5xTB1NFIETE/L1zULjH5VIPu0kn3T9KCiNTQvB5pE70o7UEijPPNOJwvWm+v1pH+5SG";
    os << "hMdKOen1FKe1IvVvrQMcnAx7U8YHJPFMHUfShvu0DR//2Q==";
    os << ") -440px -10px; }\n"
       << "\t\t\t#logo {display: block;position: absolute;top: 7px;left: 15px;width: 375px;height: 153px;background: url(data:image/png;base64,";
    os << "iVBORw0KGgoAAAANSUhEUgAAAUUAAACXCAYAAACC9UnNAAAABmJLR0QA/wD/AP+gvaeTAAAACXBIWXMAAAsTAAALEwEAmpwYAAAAB3RJTUUH3gMHEhQHkf+gfgAAIABJREFUeNrsvXe4XWW17/+ZdfW6e2/pvZIEAgkJodfQBUSkKKJiAUUQD2ChKVVRRECKgJBQpYWQEAgJ6T17p+ze11579TbXbL8/8Pjz3HvOuadcfTz32Z9/VpnrmWs+Y47xneMd453vFBhjjDH+j5R7nLEz50wMOS0Hp1x5Dv5QJW17d+LWcsQTRV7etJOBkSjdwyOCPWau/9EIYyYYY4x/P0LOnT/N/vZ5i4jbOayCRsrOM33cZMTBCG90pJgUsjnn6u/gdJfx6eqXWP7tuwTN+rcDThDAtmFMPMdEcYwx/qEoUaR74ha3WKb5b/7m2duvsWeEi7y1t5ft7SMohSSBgEp7xKS2KkhIdrCvy2L5zBCnnXUBM2bPoHtogDNOuZDDmin8r8r39AWn2b9c8yE7UsWx2BsTxTH+lvjdDkIO1Q75Pay8ZAWDR/vp6u7nwMEOenL62Hn+V/jV1y+3+1Jp7n72jf/NPrIk8eSNZ9rF0S4e+jSCmRIIeQDJRTJXpKLUTTyZopi3GRmNcPK5lxBySYz2t/LA3T/hzRee54ePvbBjxLDm/fM+Vwbc9s1fvZzvPPY0m9Nj52RMFMf4m6CIAhdM9dh/ane+0uAsXqgb0J1KEawIccn5Z/Glrx5P+9pd7GntxxVqJNbXyVvb9tHZ1StouvGX/YiC8BeHkCURRAEQELCxESgW9f/2cE/4N97bf/5s/9X3Nn/74eVJ48rsJx74Kc3nfEWw7X/5b7/+8nH2gdZdvNNXgs8wyFoatqDSl8rS4HWAZiI5VArFIpMm1rFpzxEUwaSkpIJJoSKvPvcsC8+4kp2Dw4Jp2YjAqm9cb+cHe7hs1dtjcfcPjDxmgv+5VzNJFJnZNN7+rGOYBlfhwqxuIzmdTPeWICsuHnvsWWoUnVPOnIGoabiJ4b/gas7LpHGGq+yP1nxEx44uqlrctFQEMFEJeoOMrzQRPEHwVCNaBplkgV2/f45vv/juirhtr/3vHLf9169/VkIBsP6X7X+1+W/GJ0dHhIBdsBvLS+zO4ehfhOrCGVW2oqf5416FMl+OtGlRkFycYxv87MeT+N4Tg6xuj1HtcTCkqXgPdvDF00/gjbWfkU4k+DiS5Ylnn+GNR++k4aKvYVomZR6nfe7NZ3Djosv//6LiGGOZ4hj/PuJ/MkOaVCLbqmcCipUink4i2hZFyyTs89E/kuT7P/s65UIG0yhQHMlQroLk96KX1lBeUU7V1AVIsoyRLRDv6wM9g1Ixi44DnXS1d1A0UtQ3BDln5WWYUoHO9W9y/3fv4A/dqf8n/MYPPHnxArv61AtYfNXNf+kaP3D2RPvhdZ04HE4GDZELKspwKDF++fZzJPJuQvJR1qxv5b6bnuW+WxYwpSHCCy9FeaXPw3BXL1HLwue02Pj8o1z7g4d5c/tu4dUHb7ZLd37Gt97cxrZkYSzuxkRxjP8IkiBg2fZ/WBRFQWScr8z2BGXy6RyWYOKUHaQMWLhwAopXoDxUgpYyqS21qVOdxC0LyRWkpkzFW1lDsHkmIa+HhOni3guuoK2nA0GDMp9E3bhqPjs8yOypk7nzyefxK3l0ReLo63/k6z9/lrbhqFA0rf+x9nYpAtcFJfuhD19m0lnf5FB3nyAKcFxdmT2Q0iiYNidVu2h2e/jR0ytJWSsR5CJ2sgfETvz2CF2tfZgjCYJWklvfzfKFqT6uWd3BcCzJD687jRNnLebFNW/z4+tP5f6vPcLqWJb9sdxY3P2DJydj/INg/icEEcDnEDEcEj6HgiQLCLpIwtDwOWzskEImGWX/gTZCngzOcBW7RlIUJJO6SSUMqE1IoTqKmsW+rR+Tz0V5aOP7vNk9wAUP3ou89Hg+HSoy75IrqJg2k1vOXsiLqx9nNJlm4vITWf/pn/jgpXvsG8dV2f83ajANHpf94pfPsz3i388lVQs0ySKx4VVWP/ZzAgE/351fYScQkSWBaEHnugVlHNsiEusvoNhbkLPv45xUQfL1Fziy5zCZPYdwqBkG0hruHCRHs9Q6RdweDy9/sBtMgS9ccREb/7iJ9VmTgOIYc/SxTHGMvxVBt6O5oX5ce3/7Abw+P7ZgI5sSDpdMRVUFghNcgoWv1Icv5CUkl6GLRSrLa3H5XBSGRihtrqJ5xgqGhw8zqakJ1eVGysQomTgdwcihFQrYskQxZzPQtp3RyD5KaqdRGarFpyokBJH41m08+vjveW5r23/Kn05oabJPOmYmp19yFrMXr2DVH16gd+N6bnr5/b+bX36l0Wu392Z4/JffJqJ5WPPe07yws0gmn0USJF46vp5MeZ76iY14Z4ewXu8kbo+j9NIw0sBhCn0aBz/pxulW+WhfhpggcTSl0ZYskjM03nvqFzz78NOM13r4p3adKU6Zj4diY3E3Jopj/C0ol4mteumXod8+8w4fffwRDlXGthRk06CsthqjkCWnF/A5FCrqyqmbMA5Ts4hHIyioKKEAy4+rpP/ICCWNszjY0UZzy0RqAxLOTJHqky7AqaXJZDXyWpyAMwCmRTo9hNshEHJKoMtkcEIuyfBwlCcffIp32o7uGMlk5v1rx1zjdtpzZ07ln264hjlnLSMnymzf8Bm//uFduL1ethzu4EAk/nfzy2/UuexB0c0JTWVMqHbzSm+OdW0jRNNZrp/iY1xRZs7F1SiygPZRBO+ZpxCqzlGMd5Pvtogl87TtHqErkcZyhtjYnyZnFonrNkPJPD/5yhfw6Tk+WP02WwUnc4NeXj7aNxZ3/8CMdZ//BzO5yh+KxxI0B20Ohr0kUlkEw6YgwHBvP16Hiq3lSXsDaH0xEl2bSOYL2JKAr6wKdzrPL3cf4bhTJlNdUk/ZYDfLz72Ud+++CQSFLUc6cDaU01g9joCnHEuPo5smXqefQnaUztEkPqeCahu43eVUNtbxsz8+zt3x+NxPn/yjvWrHVnozJl63m1IFvnXlhQQnNOOaPp+BTzfx0+/cxWdvvYFHVakorcQbdtAd+/s2cdYnipzot3h0a46HTqplULMR9SJep5OasIN4XCPvqiG9tZPq7x1P9r2dxCIqppAn3WWiBGSsvMYxc+pI9GXZlVSQEjoZUcAhKXy041Nuvfgyfp57A8VKMGPuAl4+2jfmvGOiOMbfpAZpSQwlVfYNx/AGSskU8ti2iFMS0RBIaHncqgOHpZPpTpBURZxeB/3RLHa8m8qAi2B5mI/e201Xe5awV2Xvjl0svfpmRre8QH++yIG+OG27X2HylFlMaxmHz+0jlyliajqpgoVuZXAKEpLfiWkb6PFR3A4Xp91+I8tzGkVLRCokKSCRl2x62hO8dNKZJJMR/BisqAtSr4h8ZmiUeFSyhvl3tWGp00HU6yaUt9iS0zk6YmHZAg7ZIpt1cOkPT2fw8FFM3WTwF5tw11cguCWsdAlaaQbbtpErfAz0RTF1maDD5thFPiIjAj/fEaU9AUooyISKEIpZIN63C0GUsC1zzIH/QRlrtPxrNQUBptfKttch0FQm23XlDntynd+u9Ct2qVd6ubLEZXucMookoCjSX1UiBED6K7NK/63KhigKCP9OhSOez5DLpliycBlu2UK1JYqmiS4YlMkK1aqCz9Y5PJrg97cu5LITm+lN5LntouP4zTUL0UfzRAYi5BIZ2vfuYziZ491X3+DjTa9jNMzGU+qloTaMZbvo7e0jlc2RBwQrQ7aYp2CKyC4ZzRQYHOlmaDRCLB1jNBWls2uAXD7PaCJJb6pAf6LA+rdbuefii8jmEtS7HDQGPfhlGMlnmTWuiY93HsHpUHEqMh6HggTIooBLVXDIMooookgSsiQiSxJOh4rboeKQFRyqivLnCeghn/skVZJwqZ/vSxEFRFFGFkVkUf4XFi1aJiFd5uSJfrIZmdFYHEEEo2AxebyCWOpB0N24ZlVTd8Vi1IklFGwfBamArBXJ9KUJZXI0jK9mwlQ3AZ+X4ypUJjsNlraE2dfWQTLWy+xjT6TWbZEoSlwypeVvNklRACQBAi4Zr1OirlS1ywNqrNwvxabWOewyn7J9XK3XDnjG8qGxTPE/gW0LnDxJ5tIvOu0PDqqkMyJOBHylNpls4cLSUj8Bn213dSepKavCcsGmnV3E0jJLF01g4YxJbNqznWNmzqPgsbBtHWM4gVjuxVk7Hj02SqG/B8e06SS2bsNTG0TvSCI3VeNyedBTCUrcNit/+Bmd3Qnhy2eMt6/64gRMxUN+fx+KrpLRi+iGwhOfbsWXHaK3uw9BUHAoCtUuDw5b5+PhBE9+cwUHjwyxe9cAy2dUcOcbBWr9Sbx183j8mkHOeGKASneS9gGD4ZEc9U1BsvkZGEKOkKRQUioye0ozG7Ye4EDHEerTMaqbJqHIFlrvfuKhBQTUUbwKoJh0D4yghCdS5c/SPpxBkl1UV5fz1Mv72PLrOwnVV2FqRUYQQRQpkxVyuky2JMzr228h2m3bhpxDcjUwsu9jJEMjOH02xVgbeiSB5FXpXbcVy+WgobEW99SJGDkX2f5enCEn9z7wDqdfeDozJ5SDR8V0VqANHWZg6wHKJ0/EyObYt3knmWyeVKaAN21yzfQgk37fzuK6MiRTQpAhEA7xi49S/Kx6F54SB8r4Orre20u2QqQai0jcxqm66I4McPatC9jwi+1UnzGLrxb28vymAh22gGpJ2MCRSIap42t59l2d8gYvV86p4+qfnG/LwRDFoZ3YOQOGe8ChIDgElJZZ6FYSvbWTdCSK0ypQyNmU1pRjTZiOVUhiFyrIp7roOdCN7HQT9LtonjOL8pBM5+YNzJlVxh1PdvGNs4PIoje0Y98RKmobmTgxPFczYNL5r/G3nx7/PzQpGjPBv20aSYSTJqv2dy+eilRWwd2/3YO/OEJX3KJ31GRps0C8CAG/g2UzvdRUKew+lOEPG/MoqkwyrdNcFSBTNIkls5iCREu1l2SygKqodCcMQrJGEZGJVR4SOQtJVrBMgYBXpG2gQOdAXDhmYtAuGiamoCJaOtmCTSZbJOxUcZc14zRTxEdGKeomgiAj2Abzy1WuX1zLT7aN8tC1Uzjmlo10/nEJ4bPXMH/GeB48JkGxfib3P3OQde3DPHLjPN7fGOXTfR0sPfsUmiaV43F7aK4Okk/FKVhetu4/yjEzJlBfV4NZtMgkR1C9DmTRRZnPhZkZwV9aQjwt09YawVnRzI4dnex/61nkwigESvCTo6CB2+2i1Mxz2kkn4Dl2Kjfe8gv6Illqq7yMJjSmj6/FRMU0NDK5FEOxHAGPC9nSSOQl/B6IpzRiWQ2HZDKxtJybLjmL067+KiNDGWTZxD1hEm/dfQdi2INk2Gxas55Xd+/DVxJALxrIbhcr/SKXnxDipKeGOXdeCa/uiBByiKhOFUuWKOTyZLNFblxeydfPq2XrtgLFTIJPPopz5e0r2PPTlzjmrqV0rRnG44yw4tkCpR4Dt6JgGAbRnMYV55/C0umNdG3vZtv2LeTrQkTy4FZMtGKecDjAYN8AhiUyEsthINMQdlEACsU80bhGZciBppn4Q0GGRzPMmljC1EqVs89diDmaRqoK86tHPuDE2QHe2tjOhweLXHtaJWv3GiwZL3H2ydNwW1nO+/GOHSOp4ryxu2rGRPG/ZBj7z+ap8dj2I18fz6G4lyc+6KWzM8qCySXU1pez+v1WZEAFckCJBLVlEPJKzK8X2Rpxk0pbTCgrYuoWAxkvffEsomXiVS0iGZHrrlqMX1UR3V62bD7McPcwhq0QKRbJpJI88OOL2bG1nbKyAL1DGezRGN2pPEPdg0xvCfFJu021W6BzNMXkhjL+6aQKetIp7vsoQyIySqyg4JN1QrWlNAYcbNvez60XNXL+jFoO63kuv38nJSEvj3x3AmfctIVxDTXMOn4KjS2NOFSZ+io3+UyeVMGJKMqUBtz09XRREoC6qlqiWYGwC1wuH9GRDB0Hh5hy3ExW/XoVe/Zux4NNUZLwiuB3OinaBkVTY3rdZK656xJuvPj7zLt0BSuWzOTBh17hyitO4Ee3P8eX57hZc9SiUDQ47ZSZlDXVkI/mCIwr5/3VW5jaoNA+qrN8ylKmTJrKrsN7aGttpWgVUVw+uvb2smzJZHr39+B0OjnhmhuY3VzPw089zJbtW1CKWapzBj9b7GXKKxlOm1nB+3sGkWWRiV4FTbDpzebxSwpHExkWBwOcPtFLIRqjQ/Zz0+kNXPPITu75+lzuf30bb3eqlHqhxe8mkUvjkGVGshrHLlzA9CYfX73qu3zvyi+QDkiUKnn2D1qc9YVTSMdTyKqTPYc7cPQfpD1tk87JpHIWp88pob4xzKe7+tjepXPdcTI7e0wUdHZ26hQNyGoQVCCmg+RzccmJjXy0qYsj0Txv37ccr2pyqCPJdY/sEkQErL/cYD4mimOi+F9AkaBCxl5yTBnHNWuU1Uzkpoe2kdfh2stmsuIYP7u6bH7zu42UugUsVaHEJbO3M091ic2CRomOIZvFEyTebJOIxXX6MxYByUYXBTTLRhAVdMvG4xJRnTKFeB6vQ6A3DQ4FnA4FqWiiYxPyQHvUpMYlMq/SIqaBt3EGw0c6iBkC2743hVs/TNLWHuPoYIoSp0TOtBBQcKgCclFHEmBXSueZK0qZOPEEYoPdnP6rbZw4oYLDQylEQ+OML19MIT2ErDiYOW8urvwgo5qLRCLLpMmTyBQ0PO4K6sM2Tn8pg/17sYsxAuWz2fjRdj783Ut4fQ4GEhmckkipS8ZEJew2meZV2HYgyu3PfJ9f/uQ37B4cZnf086Un/A5IaxCWBEzRpjEImiHQmrARRBGnaVGUZVQMplWGefiOm/nlqlcx4wM0TJlGOFiBx+8gGYmiZAs4QiUItoYa9HHw488Y6Yzy622HeeZbK/nNn9bhcirMLKliZV2I+3pHGBjOMZQ3uXt5LZOqUnz7tSL1PpsHrlvBgy98wotdUUKeMBfUBejJFcDjYPWOHqr9DuZWuPnatDBfW9vBtFI3jR6BuGbQoVYxs6WCM+eOR8v5efbZ+zmcg96cRF3YhSDLRJN5Sn0SWi6H6hZJRC0kRaI3bXH7RU30do3wcVuOSVUeDKVAz6iT8bUqaiGB1y3x+t4irz3+Fdq2bOR7z3fhL2RweEWumQ1RXebBDUVBFsH4e6y0MSaK/+83XQKK0Jwo2h0/uLDaPrRrlNZMkVhG4psrffzk5RRXzBMp8VioqsTbm3VaakXitp8frND4zQaNM2Yq3P2WRk1FkDmVRZprLNp7VYLuIlVlKh0DAg3VXgbwI0f7CZUpdGct7n0xwffPcFETLkPHgeQO01jhoqGlkqORAu6KME0lGZ598SgfftLBSNFi3QOnc9Zta9GTOrqWx+9USeY0/E6FRodMn1ZksGBiyCrTVZM588u55cIWXt0sc90T71Mf9CNJMqUON8dfuZiR7lG8bpi/eA497UNEEymOW7yc4aEBGhsm4vWFkMUiIZeMbugMDibp7TrC+mde50j7IZw+P16nC0kQGecVafHZ9CVVTvvhd2n0ZdEcezB1GZevDAMvzWVVfLBuLbMmwtDgKEe6HMxvziHWjiMak6mrC9KbiFGyvotHD6t09bUyc+ZUfB4vqtOJw6OjFZ20rtnJjFMWUEhlSCaS1JSo5HICckHnwMbPuPhHT5Luf5zta/cTaYsg1dWxuS9JMqVjiTJPXdLMtFuu5MA1v+WjtkOsKqoYgxlmz6njqbsX0REV8SGz66FVvJNWCHpdbBzO0xyQuWBRHWY6TySS5pnDKdKmxOTJkwi7VP7pRz/g2Xt/RiyscUylxoh/CnY6QU1piOhADw2+JG1yM5XWKMlhnUUnT2PJRc+yZtWX+eb3PuYPL17Gtlc2kJa9bHxjNdUNYYKqyO5UOflkhGhK5fZr5nPdD97gl3ecgd+lc8kdGykhz86oPRbvY6L4f0cVBRuW1Nl2pnwcX1lu0X9omPvfzrF8Ery53wZEzp0lcP6xVQgeP6veOsh3vjaJr/20g4BHZHqzzdY9GmesqGDL7hjdQwa1JSrINqmUTcgvoiouvF6ToWgRTbeZ2ezm+c80vrzEwWBKIZ0oki4YJDQRW1DRdAufV6YgOZjgD1AXELigJsVDn4lMDZu83BYnJNqYiFS7RFKmjduyaS9AXhRYFFQYLghsH4gxeXotN10/n0ishgfv/T2KCpLi5axvnE9/6yFKQ1V4ywKUBVSGR7OossCM8RMxRBFB9BIKl+KVdIKBIJKg09pv8c7zb0IuiWt4N7plEDBNVEmnxO3nxJ/9lmmhBGdfcSO9RYPDHQM0VIawBRvdtKjyO7G9PoqJDL5wmPaj/Xi9AkXj8263L6fR0lhPIR+nqaGOysYqvA4Xlp1CliU2bezinC9eRHxoiPRIJ0O9EcY3TmDPQITmeDdKOEz0QBdzv/VT3vvp7fQW4oTLQmzuTGOaOg6Hk58c66MpYKJ+8URy2w4R+WiE2dfMw1VM8uFLMRSnztRjy+n6sIeHDyQIZBNceeUSXL2dhH1FNnUUGI0X2ZctciijY1gB4oUirz38QwxRZv0zd7EpblFZW45sFSiYEpJRpHdIp6ZSIpsHQXQSCAWYN8nDe6/t5Lk1DzJz2g289IcbkUb72bKzlYnjJhKaVcfsRfdT6bbwhELMqXPg8Pn50TeO57PDOV54chVtoxZHR0zBGssS/49IYyb4j1w1BBpLHHf0DWV5a1OC8+Y6GYlpDOteLlvo52uXzuDRN/tIaV5WLm+hvaeHH/w6Qm/W5NqVNTy6ehR30IutlOEwc7TUOpjSXElJqR9L13G7XBx72hlkEnkEt5sVJ51AITiO2qDMsXMmIctB1HAFzePrqa2uw+HzUlkWwFdSis/vZYaoM+gVOTbg442Ek1ZXA45ILzNcIjFJYroDCqqDoaKIgsB4t8ywpTBSLDL9uLlE2nt587X9bN2+A6/ThY2FqoBtKVhOJ1axgKgGaRo/nUR/G2Yxiy0JuIPlpJJDKMg4XA6wJNxuP+ObSpm39Dg+eWsV3nApqupA1RLIqsKSqy6jsSbA1268hZjTTUNViFOPbaSuthktl6CluYWZ48uYPb0WyVbw+FS+9YUTaD3Uz6wmF7PGtaBIFnaxSKCkDJdLQrAlgl4bTbXYtzXCghULGentQvDIRPtSOBQRd3UZxUwOT2MzluhAqKhnx6oXmX7779j/yu+QAk56RosIiIiCSUemSKgoktl5EDtrY+kaoWiOu1fHuOrh0/HGeig54RyG9m9kTyzPz69o4pn1XXzhgin86oU2evIC64Z06qd7ae/KksgZzBhXxe69HZxy2inoPWmSZh5HuJqaihpypkpDfQmxWIqFs6oJBUOYuTRbD0XpH0qx43CEo609JPIadz+6huhAP/NnTaB+SoD5J/+GG740nZuvWkzbkMX7e4a4ZaWXkZjI+be8wfKJHjqHCkQL3DkW0WOi+N9GlQVObFHt5UvKeHdzDMOyqKsKMGXRRB44T+K2Pw7ywpoh2h6fwhPrNTqPtlLu97G3p0CVW0CxTWZXe/nqxeOYUGpyyeUNPPCbLt7dG+OeW5YwvbGEBbPmMHU+9G3ZSFNTCI8Tbn/wPR64bRmaXqRESrNgiodbfraWi4/1Ui4VmFUjsWKeh6ef30JXTiNsGFx0YSN/3FbAr6dJJJI4nE7cssKerMVw3kAyTQKqjSAJOASQG+uZ1nuUnGXThMGy6RMoE3VURWJAExH1Iu6qKmK9EUoqyrEEk7wuEu3voKLEg6gl8XgDdIwMoOd16usbGI0lGBoephgbYcnpZ5AXbVz5PA2nncOXbr+NyppqPv3lLdQ2yyyf7uGuRzdiqH7Cisahja3c/K0TaXAbHF/voLHBxd33vo+zxs14KUXr4X5aasvp7ElQ1VBJwCdjWhIOj8Ww5mVkwMmChZXkUjqFbIaBgTgCRQzB5si+I9iZIpmjHXhqaoiMjtBy3GKGNjyLcOo3ObDubTRBxRZMHLLFaM7iYBYCsoyzfYQJx43jD+/1c9mpTVxw6Uv09KQ48OSbfCh50eIGv900jFE0iR4a5oRKN69qNZSWZulKWgz3Whh2ERmZnuggib7D1B1zDEp0hKM711FeZvOVm0/FERnhrIvm8c17PqZ72yEWLKzhzuumoVbMZV5Vmhc2dNPiTuKTBD44WuShn51DwwmPseHdh9i1uZOv3/8u9325klR/L8su/yLnX/8HqrwiZ15wApI3gIp5x0A0PyaMY8Pn/xqiIIAgcO0yn727XeWWs21uXZ0maJosOy7AL99JM2N2Bdcva6bCPsJPX4qwd1RmNF7g+qUK7lIfb29Ks/rblby5O8fLnyQYFxbJajJfWBbgSERlzUENURAoD4f5bHs746odDMY1RMlidnMFPYksYZeDWDrPaCxLOOAkZwi4BRtVkUhoNvPmNKAN9GO7y/jpl2ay7IZ3yasihm3gUVzo+RxTqoKUdQ4w7eK5vLupF3cgQCKZJdEdo9xdxOvwcNk1C2ly27RtGeD1XR10ix4Er5eKSZNx+30UMnGWn342+eheYof2UdsyDrffg9NXh9cXoKAVyWYzTGmZjogAkk1+uA8NBbQ4DU1TGNfUSHz/ep55eTXvfLyTjC5x3RdP5NVV6/AHg4xEE+zszHLCDD/OUDU+2yBUGiYz2kfHcI7xbi/701mmTplAwOtGkhRsBCwsLFNE9bqRHArpkQJy2AVFyBcyiKaBM29ioeEQAVlF8KiYog/bthlfPZP7X34OYXQEw9DwelzUu2xaMxaniTmuvOMHtG5dwznfWIkZnsbA66/zu7tX017UKKlWUQyVeWUWdbXlPLVrCHd/hB+uuZtjFvyA0lAZpqAjWCKJfI6zFk7hUFc/13zpJCh48RTi3PHcahYe00LBcCKlM7gCKt/60mz2HOlj97ajtKadLD2mmdPnNLDrcA/bDmUIOUvYeGAnwwcHCUwZx0++OIe5py3jW7e/z5PPv8bBD29i2ck/pyb8YwZzAAAgAElEQVQEV123lKaaen73/Ae09aRo7c8K/7ym+hhjmeJ/+FJh27Biqtc+NOpl+1tf4OzvbmDx+CCax8eufXG6kxbv3tbEx5vauOG3I3TEwSfAK7dW8PrmJDNrJI4MiHzSDrs6NZbMbWbzviiTx3u555URuiJZsCCbKyKZOtOqdXwem6YKkQm1TqyCzvwZFXT3pJnUEqS8xIVTkQjLEqZpIkkiQdmitSNJMOAGl4fjGgNsHHSgJtJcUKnSmbOYtewk2toP8+PvXEZzAxxXGmDRrArmHNPMT26bz8y6SRgOmVzfCE1uicfWHCLqcCEIKnnLwtR0YpFBhKRGyYQWor196IUkqgyWKSNKGi7ZScEQMGw3mqETcCq4HQ403SbRuZtQzQTqa2vQc1nUxrm4WvfRkRyi2ify/vZemitcFHWTpqZKjp0YIGW4GO4fQnbJjHT3kTZ06n0iEcMi7C3D7VLR0wVsUUC3LEQgpxkU8kWG2g6RMW30dJbR9v1Ee0YwdIucbWG5XGTzApZkkctqaNkkfsnHhrd+w0lnXMxH6zbgcLgocRlMLXMyq0wlPZDgwh9fxuCvnsMZdGKPbMIz92SWXjeNxpFe5IiTEjcsO62BTdtHaCpz0Z8RufTy6ez8ZA+ZAmAJKKoT0bawZRmfz4ua1xiMduJ0hrnhomWs2dZGe1+U0UyORLrAo89uZV+/xpSKCqoUESuX5VsPrKU7WmR+g4vZF36Vo7s/4YZvf4my/nVc9sAehvfu4/tXL6ZrME6kJ8a21j5a997JovOf5ooVNZTNXsBMc4gRU75jYDQ3ljGOieJ/prci8MFvTrA3bkvw4R++xCln/oKMJtPYGOaNT/ood9osrbN47O0Yrb15StwCH9zXzO83JDly1ODUs46lNFDCth39FHSbrR1Zth2Mks5Db5eGbdmkchajySKxtE5PNEvPoEl7v0FXr8H+jgI7+zX27I2SiObZfzTBvq40scEs3SM5SOuMxD8v5GcyGlm1jHPmhsmWFMn6F+GodrJ5VzuEA/h7d3PZlBZmN9hYogcKQ7yyNsqegx18877duDsPc8rdt7Pt2Rd4rdskqlukshoOp0quaKH6A4TLK4kNjzBh7hxUO4EllyCLJpbDSWxwBNntBdPAzCWIxhJYgomVL+BWLGxspEKE8uYZhFwqnds/ZeGNd9PVk6atL0c6nqOtJ0P3QJbWw1F2H4kz2JsgGy9S7E+xN1qkf1ijTnGC24nsClDdUocNuLxe3H4/R/f0c+x5i9nywXau+sXjNNQGOPb8i5lz0pksOvM8euPDJPr7iCVSSG43LpcLQZawZBlFTOGrnIBbhoKjlHQyhkuWmFLpojMjML9WZsbSBWzYcJCqhjSOvEBi7zqMvh7ECpl5SxWmLhnPhrd6cDl1dEklr7lYfHIt73/Qii9dQBclCoCiOjjaE2HBpDr2D8SZM7WZjqNHiR/YyVeu/BqbtrTRcbSfntEcgiGQimfZcWiI99tG+GT/CDPDAidP8ZE2XbiCOidMr+byax/nvGuvIW9ZVCfaWJfzEzmwg+ZgDCNXQBEUxjtE7n5+C88/diVP7Svj+kvK+GzbwB3JdPHOsWxxTBT/Q/zpkYX212//lN89/WXef3YVj3yY5OZLjmd36xEumy/i8jnY0WlQFrDw1rfwzWUyF9/Xy9UnlyGaeX7/py5Wb+olmTVJJnTqJBEZG48goAgQlkRkQaBCAqcIfkFAlWzqFQG/JCAJMEsFE5Fax+d3UpcrAi5ZRBYAEZx/frCUIgk4MJk2uYwdHWUsPH4But+Ld98HBKMZFNXH9x64Frn7MI8/u4tvr++nwVFk8rITefTa8fx+Swc3futSXnnrIypcHnZGskyocqA6nWSMz/ef0TTcThnB4cZd2sxI935UbykBpwOHMwCWiS278Ho8xHs6cRsmhmBTSCfRESiraiYf7aJ60kz8DpU9H7xPLJahQhFQJaiQJVyqQFAWcYsCflnEJ4kgi/hFEZ8AJdUeHC4fottDNBrDtCCXTHPiyusZiW1m2tKvcfF1l5Eb6qK6rpnE0DDZdJa+I9uZNH4CZRNaUCQBUYTeI0PIioXPX86oBe6An8iGd5h19Xf54NU/4nYplMgmHQmdcxbVUtrk5tCmQ/QdHkXO5RFxEzs4gJTW6e/UGDiSJ2XmKSRMotEkcmkZkyaWsG57G0IGcrZMQrNorCwlmsoiywqCaFKuyriCJfR0DzPSvpNLV55GfjSJHBvFQsAlCBgW+EWBgCTQnxF4pzXJnoNDZPYd5O2PO3n+11/kgQdeYv7kuUy8YCW/uOtpls+oZMf+GK/87tvc9IsPuOGLx7L2gMb9D7/O3ZdWcv9LMe788kQ2HozekUjk7hTGqmhjovjv8cxPTrIffno3jz50G9vWvMfdzxzmG5fPZe++fYTCXm64ZDr3vdzD9AqT9w4L3HbReG56+ghnzPDw8idxDvQZ+ICwLVIuQkD8XOREQSAo2oiigEP83NHdCFQr4ASCokjOtNFt8Irgkj5/NIFmglMUcYsg2QJORUSxbIoW+ESBvGnjK2oUamYya/58BhIREof3cu7559CyZAaz5o/jrm/+gkxCYp8gcevMFjb0pbnrtmm8s2ofQn8ni69Yye7166ixCgzmFKa0+ElbRaJpC1EEQ3BSUltCVVUVWUFENkCwcpj5NOVVJaQ1ASOXIK8VcCpeUvkCPrcHEZNQ0I8sCJSW1VAc7iI0YS6OQ5vZvbcdVRJxKCImNnbRQtNtbNMmGArhkwVKfUFM22JKyEXnSBxfwE/WBgSZqtpGvE0NhBwZjj/zespCBkbeoG8kjmVYJNJZUvFeJFEmU9BJJzN4/CG8pX4EocjAYATDzOAWLRKWk6xbJlTM05c0UPIZZMEAy2DOhBrCFT6yRyMUFp7GaGwUJZcHlxtD1xE9Ipt2x/D29HDST27n5796m5u+fSJDu9poT47SMyxRMEzSRZjSWE5fJIEgQFnYTyqfp64qgFd2kY7EybUf5vTTV7D7cA/RVBZBsHEKAi4BFGwkIPzni+G+lEmDO8dzf9zBuSunUHSodG1cw4UXnUv/4W1kLBPdX8mv7v0RSy68jccevI7P1u+jdSDGa09cxclfW8OaNx9j3fodd0Si0bGh9F8xtiDEX/H6fcfY9z2/i1sfeIj9z9/D9144Su+q06m9+F2uX1rOnbetZMr5T1FqGuyNSjgkmz0jAgsnl/PqlmFmuAQqnCDakNJsCraNaAu4HaCKkNRAtkCUwCPZHNVt0gY4BYGwbGPYAqUOyBs2eQN0wCNBzrRxija2CCIWYRdogoBp24QRiaUsSj0qWcmmv3eATCRPgytLOtHPey8dZM75Z/Lqa2tZNK6RI5E+br1xGRuf30Z5mZd3hQoS8QwrFjTy9cd2sLQqyNp9Q0yq9mAaAoYgIhTzKKJKPJai0N1NsLwWXTcZtQTMvjz1Xo2MqdDZ0UedV8UyDTqTcSr8fgrxYVpmLaJQzFFZWkrhyA5Ouu5GPth8lKNd3RQsi3KXi7rF8/jSzd+ntrKcsubxSPk8ttePrGsIipNDbTt5+Ps3s2XPTm6/75ts3trHZdfcQNjt4HDXPnqPaBS1NJZuMZJPkUimkC2ZRDqLWYxBPoXgL2dqyyxk1YcvFOLD9R9TV1tFuSNC0eFkV8dBTj31VN5/4VfkLCeCJNMZ15hetBhO5Fmw8nw237eFza0xnAWdWsFHOqBQmYLD3iYa2toYkb2EOg7w3KYj4PcTyWg4HSKKJHCgK0LI7yarFbEtiaHYKLlUCZKikIvneXE4y/Yjz/Gday7nwSdfpDOexfXnBZcMW8S2QccGW6BJtOkZgrgksundXQwFGjl1chV3/vw5akrdfGOZh4cefY1JYY2u9Xex7PIHuOHC6RwdElhx1u3sXvsoTcdch5EZEtwOhZymjwnAWPf5fzfFkuMa7ORQmrWPLGPmFW/z+PVlfPXJKD88w8OUOZP49R8OkuuPEq72Ei/AjBqLj/tFDh3McV6TD6+dB10nmRUwDRunG4qAmRdIWgpBl/n5PDhZwO3zkM6aeEQTj9silwXTAo9LQLMdpM0CFHLkZQ+Kr5RyWcMWDXJZDQQDSzexBDAsCy1p0HDpl4jlC1x70XLue24dxvMvsuSUKZjOIFfdez09777Ehp3DaKabSfUmA30RrObl3HvnEzz5u6spDCX52e83MaHS4LNuEYcokdY0vD4PpqiimhpNxy0i1deLr6yGyoogDp8bd6yH/kgSl5SlrGoc+1sPsmDGTLREnKYqN1NOuBCnU8SVG6Bi+vGY+Tymv5H2NQ/Rt72f2vOv4sjmtXy45m0iso/evm6q65pIplI4fEGs+CANTZM5/5JLOf3M8yikEmTyQyiKk6N9gwwm8ghGgVwqysGDBwmWlJFMxHBbOYbiozgkmenTl9PX3wnFOKIrSFVVNYd2rUNRwmzYvB2PI08xl8ZU/Yz3BHlv826mlzrQzCKNFeVcPc3DH9viDPcVufqLC3E1TOSNF9Yx3pHlcCSND+gYLXB8U4AmX5D3RlL0GnG6Imkm+oJs6MuAIVLQLUrCXkYTWRqaqnFToNzjZlJLNf1tnRwpyFixCFVGnnNPXcqv/vAOpiyimzZFG/ImmALIApi2gGWBJdoULJjWbDMwCo/8eglLL9nAoS3f4ntfeZpdQxrr/vRjDr+3hhff+YQJC86lr6+N0xaGuOKObTTWVtDRMyRks7mxbvTY8Plf4vG4uHRZ6R3nL6ph+/4ovT099GfLOHWqg1Mvv4I3Pu3jmdWtXHXrqezcNUBKg3japrcnS0gDp17AKFroRdB0qKqCom6DLZI2LWTTIhBUyRd1EjmwDINCNo+maWi6jmnoFPI6lqZTLOaxCwZOVUATDQ4dSZCJZtCzOWSriJ41UCwbWzcRDAi7bLTKeloWnkiZEcEXSuC95Fq2rH4br1ukpkVmNJGnLOAhoxdwhcpZu36AiVMngdGOQ3FTWedn984BEjrUey0GEjq2KKIXssiSglMRcTgVFKeDXDxK5bgpWIUcfSkROd5PJFHElEUmTlvEYH8fsxYuYtaZl1Ne6aHEJ+KfvARbcmP7/UhCDFf9Itbu3MfTzz7Bpwc7EWpacLr91NTWomJSNX0uflknVDsB0yOy/rVVvPTck6TTeRYtOY21O3eSyuQQ8zFy2RQd+/cQ7e8nOthPaVkYzRBZftzp7Nq/m/6eTk4/9TTeevMNOvfspP/AftwN4xAEg4qGerZ/up3yEidWYCJ6ogdQyWQi/GjeeHYNZDic0pnidnD1kjpeencfzz3zLtsGo3xwZJiD0TT7RrN0aBbru+K80jFMndukRnBRqogcHEhhGwYZS6ZgGgS9HjLZHG6Xgm4KFPI56ktKceZSxLs7aQy5aGwIMDA0wuLJ4+geHaZg2FgInwerAKYlYFs2DhkMQMSmfVSkJCzw5ps9vPXkhZyy8il+98pNfPjSZh547B1u/dl1WAmNNe+vZe6scXz5ro/4cNU15Cw/Xzh3xh1/+mDv2BB6TBT/OT/8/GnL7z14or12W4aLF/q57P6dnLBsGTWBIhedeQz7dx7k3t98zJSWEJOmevhkR5qrTq7myT/1kykIzHQLVPkEfF4b2xKwANESKFqgSRKKZZMyFNyKQV6zCaoWpm4R9ggYoo2Z/3xOpIWALQpYgoDbI5DMQroo4HEI5IoCDR6wdBFLgIIhgAUuVSCbscnXteCfOA5poJfF86bxuz9twZlPYmEzvqjR7fdDNs7hPQW6jwzQOpJBsQdpqm/m0529lAsW+4YSjCRMRgoWomUiWSaipDKaTlDVWIstqZiGiOVQsLJJZEHB1C10h5Oy+ha8psbMJSuYsnApy0+ch6jYGEUdy12BKkEsOYpWyLF76zBXnr2UwUyKgNtFKOwn5Pfglkxy/b2MJCLE9h0iFRnEWV9KSFdwVwbx+Lx0HNzP7Td9l+9++/vs2rSBvJ6lde8h+jsP0TB5GhOapjLc10s6FSM6MspwZz+iYLF61R8oczuYuuh4TIdFZXk5/pJKHr/jHgLlZaR0k5m+CF1plYC3hP1dnZRi8ZV5Zcx2mGQzeR5Zu5/enEZ3USQo2ZSUhikL+FF9Thy2BaKNIkkMZU12xNMcSZiYlkHSBMuSEAQbVZbRtAIupxetUCSXz7NgTjOjQ0nqhBTJaBKrkCRYDflCDq2QRxRAKVp4FBHLBEmwMRGQbVBlMA0BvwIHkjCx3GbVR/3cfveVXH7ZEzzx21vYvPkTVr+8iZ///EZee2UduepJHN11lKvOnkpNXTlDnZ1sPRD5tFAodoxJ4tjwGYCzT2iwy10CN11zMp9sP8S1920k99lVLD3vKT5+84dMO/0+RLvID1cG+MoLGisnKLy1N4lhCJSKNscFBBySgNMDomFjFkFWwLIEDGyc0ueLkOnC5+LmlG2KhkjBNLEtgaBXJJo0EUQBv8tGy0LBFggFBbLYdEZsgghkDfCrAhUOG9n1+STuogWiaeM9cxkzT7mQD595iYcfuYonf34PzuO+weq776HB72LpolqG+yO8saOPuScs4bIT59G6cTtVIQflXg97WvsJ63lkh0BHX4InD3ejCU5Mh4qtm1TW1eEtq2Vo9CiyFKSuuhpbt9G1LFPnz6KmPMDysy5GynXgRCCiSfiEPILDjyLASDLNhP+PvfeMt6ss137/z6izz7n6yirJykrvlUCAUALSFEQF6SJWQFQsW9yWbbBiV0RsgCBIVZAgvSUESO8JyUpZWb3O3kd93g+T993nnI9nv+/RvfeZX8an8fuN+ZvjueZ9X9d1X/fMufzsrmd587HvEWvvxvcc2ufM4cDWrRzdd4Sc7aKqKprmIxQDRULVdcDzmTenmyWLFmHFAxSGUxzauJFvPPgsG+/8LGd/8qsMjR6FvMdgfy8XXPkhHrvrl2QLBcLBCF3LliD0AEtWn8bmp+5nxQXXsvulZ7nnNw9y4aXnsu3NzUyZ2U1DKEtAAaHN4MCeXRRdnybp4QYU8iWBokrC0uDLV0/lqcpcwtUMlUKJyaMj5KtF1i5IsGlPkmSpjKYqqEJQcWRtEAAdy3GJRkNUyhYNdTFcVVDIFjl5ZgvTmhuRvQMkxwdobxekU5Joi47reThVn7GSYDKjEFF9chYoClRsga5JdCEoeBJfEYzaku5OQTxWh5pIcKznOLdd1s51dw8jj3yXky/7Fded08zWXDsb17/FwMZvcPM9u/n8pz7E3HmXi5rXwf//QfG/82falIT86HlTSOWi/OjLi7jsc0+zYEYDR4dS2LbGdz69lNNvfIHvXdfIw5tt3jcf/rChRLngIyTUCVgah3oTpC9wPEnFB13UMhajUXCcWmST6wtsx0dTBIYp8ADDkZjBWjyWdCW6CooGrhRY0scWgsmCICYlMR1cBEKV4ELKFSgutJg+7Z+8iUjbFMaTWRa57zB7xSzWPzPM2tMb+cbtT3Pe6jmc9YH3U3lrO2PHB9h6qIdBTUc4LuMVm4CiUDUCeOUSQU1lEkEgHEfBp1y1mTJzFtFgjInhE0QSTXS015Gu6nzg8gtobqon1tCCUhkiagTIORoJUxA2FDTdrO2zDtbx9ENPs/6xewkG49RNbWaif4D9B07guj6aoaFpSm1Nq9BRZG3iwnN9PASu7yFUWHHyScxsjzAxWWasp5fvPPQ4v//eV1i06mTUcoZXXtlI31iKW77yVUzTgUCEd/btQDouZbtKx7RFbH32WUIBm9nLl9Mxdzkndmzhib+9TEdLgNXdYY5nYqTGsqQmRtAMlZawwbQIbBlzuGB5jIvOrOf2JwTxsIL0BdmhIUY9h6+tDHB4wmP/uMpQLo/jSsq+j4KK7UlQVcK6iuvXrFXNzQ2MTibB97n+glWUx3JUjuzBkQ5oAiPgE9QErg5OCSwE4ylBruTjCfA8gSUkDaYgbUkcBGVfMuDA4mbJslMXIHTJBefO47bb/kooEePB332SVef/lJcf/AzrH3uSmcuX8t4Lz2bV5d/n4vedzF2/ffG/PSYo/7xoXbMeCKHQEOCONcs7/4+wwLddOp0/vTzCrVcE+OTXnueKs5v52RM9vLA1yVc+uowNW4ZwJTS1TiEWEBwcdqiUPKQH9QIaVchbUHAgHK6BnyYhEZLoOkxmJSVLkCtJTF+ioaLqglxFks9DwRX4LniuJG/DRAEqZXBcBSWoIRSBbUFIh5JbU6aLFvhIpkcly2aoRGOCcs8h2tqbCFLkweeO0uGl2PXKK+SGDnDW2jM4/fRz+NknbuPbD63nzq07OOwKJotF0g6Y4Sh+MIJml2gIacQCGp0NzUTiJuWKjakJTD2MjoZVrXLKkim4eh3nnb2EFrNKKBQmn5skgErSNjGDEbKlCuMlQTafQQodK5fi2SceJBDvIFEfZufmnew60ItQdYIhA9NQ0BVBQNEJarVfX9cN9GAI0zQJaQZBNcQbr24ibbnEpjQQ62zg9huv5/QrP8s93/oJSqSJ9132Qb59z29QQjYDPXvYvXMbuJAbK5EbSPHyY39mzsqFLLvwEixpsmPD3wk2hqkLR8lbgtGCIJUbRw+HqXoVPM9jMF/l5RGbxojOQs3l1fVJ8tksxUKBQqaE60FE1dnSZ3F4oECqWMQ0FYQhcDEouz6ekAhf4nq1wUQpateAGaJcLZPOl/BDQcy6DlQVEvGayuxqAteWCAMsS6CoPrGIiqJKVHyQIFwfVyqovo8iJS2apC8v2L2zh3PXLKDnrW0kpnSQLVeZOyXMSTMjnHrdr6lPGKzfeBDd6+PrX/00pyxbzmknL5D/e32Lgs7WmFzaHZO6Kv5T1GH/lJyiELUqYXqTJjsMb12yzJpZ7aF1V51ev+6Ng7nb/6NYLhAoisrj93xJPvbMRr73zSt54JEdbDs8wscvXIJW6MXF4D2LDG799QEumhegrUXlhW0pdp5wiUhBTK2lbFs+CAklByI6hAyJriko1OwTui4oVyGiq4xXwfZqAkxIBV2DeARcAcITBFVBSAepCPBq99q2R2MIqo4goEsUauBh6hLLV3AdSd4WxM84m0BMR9MDjEykGR5Ikx0aYPbyyzny6qtYQxn2TgyjBgMEoyq5EjS2xFGESpNeIVW1KWohhmyFbL5C/ZxpVMsWpWSOQDiM47lMFCt86jMfJ6/FOX1BC7GISfeS1VjFPNmyRcrXQAgaIwkUI0Y+k6FvoBcZMPi3T9yGEm9GIciBfbvI5Ktouomp1DIrdRSChgq+gy81NE3FdQWq5+MLgWHoSFXBUHV27T7MsoXzqJSqCARH9m3iys9+hjlnnMzwoV0kkzniTZ1Mnpik//BxxgeHOe2MVbTNn83Urk5Gj/ViOxXymSxOxWZ8JI9l2xRyRbRgAOk4WI6FWy4TUgVI0PEJ+A4Hki4xzeDIZA7NU7DyWTyviiMlWcsnh44WFFg+ZAo+vueBAqZQ0XUFX2goolY1Op5PLKRhuR6DqQIzWusQwRB+MkU67YBQSGfAtwVWRSEcAiS1CtoR6DoEFYnjg+OBpyg065B3IeUIihWfjXtG6Bkq8bnL5hEuTTA6lmLO9Bbe2D/CyQua6Wxs4OHnD3PFyTrfvGczD3zvAu58YOft4P1vAcRIQPDrj01fd9+rkzQp3rpV3ca6obx/u+fXvLv/jHr3P6VPcXpLUEYUQbbkUD+9jT9+ao78/M8O8komy6xWQx4ds8V/ZOWOAMJBncd+/UeSeUGL1cP23VkWzunm+p/uYWZAYeWiVt44nGNBK+xyGzn4Wpnff38O53760LvSjAQEcQ1yLmi+YCIraQgLgoqPjUKuCrYjCGkCofjUhwS+L4kGwPXB1AQTRVCkJKpLpAnZgiBng+v6BIqSRETgaSqRsE6+XMX1fIquSoMvEFJi2dARlzS2dzE5MkT77CmM9U9wYH+SO664mtceepipU2fx0MGtJBSXlQvnMK87xswmKOTTvHi8SkjGCA2Osi9dAFvFCwaIjyfJJdMkgjqoKr4R5MwFnXTN6iA0NkGiJUZRxug5coREvA5fURGOy1DfMUbsTeSKNqFIjFLFxS9YuKaCpoBPiWTJxlBqhmTF0IkHAuDZIFQMM4ymaShSUHU8pJSoRgCEQqlchECIZkXlwUef4drrLuLIvhMEU5OMHO2lZ9ubrPrw1bz+l7/w1H2PcMqa1Zx13mpczWR0okr+wDYKvoaiGBSSBSrlPIqioeoBQmETNa2RnMyg4aLoYfRghK6gx96MR0gVZBwfXVHYlS4TMgLgVYiYGioKYU2lqAlCCY2RoQLVkouqSBRFJaJ6aArkHUBxa3/6HniWDZEgEoPxdJbRTAFFSqZOX4TTu5WwIQnrkmJZQVE8VFegeBBUoaopRIOSoqUQEpJ2XZAsg4ekwRUUEFRdyTUr4I1jAbaMuQxmqjz42928dc/7CN4Dr+5IcvE1Z3D/ffdywwVLuPHC2ay99E7WnDRbbtp+QPxHAVFVJB9eEZef//0JfnBNK1o0wXcfPE6d6chZM+O8eTj3bjLfP9e+mH+iSlEhaAqaTClVAW0Rm/evaaGjDj7/64Pc+dk5DI2msGQCQ3XWZUre/2sLgQQcx+WKJe46x5OsPW0p6x7cxc1Xncx7zp7CDx45zm9uauMr9w5z43kNnLSmiwaqrH8tzcSIRVhAh1bziQUUaNKhKQRCERgaRKMCVwqkW6seG6NQdWv1bUNEkqsIEiGBZ/uUPImmK5g6WNUaxx0EIgFBLAyaInFQCYZMZNXCF4KqB/GQIBAAVUoal65AzOxirPcYVjXIlg1vkwtEmX9gL28pEY5mRpiulLnli7dy9ecvY9/LLzFcrHIw41IhxIn0BIfHVYq+IJkrcv7sdhqqWYTn0xgxyXjQ0zvAHT9bR6pokR/ZT4l6RDGFGaojnU5TTI1S9TQczaTs6FjZPBO9fXR3T+M3P/wdwVgdQVWy8a3t1EWCmCXrpR0AACAASURBVJpBOBIiETQxVQW0MNGAQNeCBEMBNE3DMFRCmgGKwJM+0WCCcDSIppngWvSNZTlp4TxSXoXhEwPMXLSYB35+F4lEgis/fhlmJERvTy9joynSqUlS2Sql9Bi+61OqFtCrLmVXMDE2jEQipUcuX0RRdKRnoeph5oWKrG122D/pkJUQUCSGECR0Sb2mEjc14lEdI6ySd3zSI0Vcy0YzVTTNxMCjK6ghfEHSl+iiNsGDJ0H6BE2dqiuIGirDqQyt8SDBRANxLcjk+GSNW9QFrgdFR2AakMxBIlbjqm0HYgGBLSFvw5SIoC4oGC9LLAQvHK5yyzWd+J5JNQtqNEx3ncOOngzf/d4ajh0eRQlG6N+1l3ltDr3HRhlJTpIu/8ezF5d2hOWslbO4dHkTuw6N8dzOPOuujFLJSfYPW4SFt05o4nbH++eqF//BnKJAiJotxtAkjUEhpy1ezIHHb2TRvBm0zF3B/iM59u2+lwkaKbacwRe+dTvvWdxUGwv5v9Z+4n9e/p0R0XWDoPr/4Cj/FzAKfvSyK2699RpGJsvccGqAG7/3EglFIxIQTGuvZ16jzba+Ejse24wj6ugZ9Qgr4AjIC0HQkFSBpCMZLULJloyXJcmswPZ8UAUFX3Jw3McW4AnBvgmBKqFoS1xF4Eqo+JKKDYYi0RVomwLN9TV+suipJC2P8ckimikIm9BsSFTfx1Bq0zNaPExxtJ9itcz2NzbRNns6Zn6SB9Ug2WoWPaCycsVyZrdXmbrsozx3KMfW/gpjnsaeg330D3uU7SrYsGxBO+coHskqJAIRxssO+dQkj7/5ImP5KunUBFr7GUgnT1Hq9A8M0HfsGMLXSR95BzXVT35wkMmszYEDBxg7MUJFusQbdfYcPEwiaqCpGroQNEZChKIRzHCYhkSYeH0Tza2NGGaAaDRMfSxWy34MqNQlgviUUBQPq1pFUaPkhycYSJeJChU7lyYzNEw8UU/79E52vf4Wzz29nuzoMKnRMaqDg8yb00jn9CaqnoeTLWBZFUqFDIqvUMqWKCTzBM0gjufgeYKy4xHQDCrhBl6/ZTZXdcYZzLmo+OQrVfqLFUZdwXCmQHqshJov0RhWaQhrRA2NOYaHEQiT9n00RQG/FpCh+AIpwPFcKrZDQHcpOy6e63NiIsNbBw6Si8aZ3jmDYl5SyEtQFDzHp+pCPA4nMlAoC6ouuJ5PoQQRVWI7khNJSVUKQu8e8GgZnn9pD88fnODqlQ5bj5e57ROnceWnX+PoiUk2bXyH1auaiUyfRcYLczSjiv+lwv770QKUdwFD/N/O1P+kvPj3u7jrM3PleR86k75xydrr57HwtNO5/bIIX7o/Tyiu8oVLmvjMdSvwhMH0poD895P7j+cc/+FPcOqiafJXX1nNiNJNR5NCXXGU17e/QyIcIZUr0hrTOHPtSfzy3g18/Qvncd33dzFVTvLn13v4xve+zC++/G26FsyiYe4pTFUmyPgh6hoD9I8JvvK5pXz+u1s5u1uyp6/AqYvn8S+33yU+c/175dxlS7lsbSNf/PEBLjxrCk/89G7mnbaKVGGQqaUkVnMnb764G1uojDgGX7x8Dp/75XZmaIKoqIFqTJF473IQQSHxfQWpSOImYEp0T0GRkkhQkiuBIxUagz6eMBDSxQgqZHM+0VBtYVOpLBD4hIOCgqvhSg9F9ak4Cr4EU0hMQ1AsgWZKTAFVWzLzw+8nIx0Gh0cpFkyKxTJjfcNYnkfI0Jna1sRn5kd55GiVEV+SnUyhKjp2tYLt+Fi+R8QIMDie58IFLbw1OMF0M0BVSvxSkqt//EtmtEYYSRYIh+qpuCWkMLErFcqTY6QrLqgGsZCJL1VGTxwjPzbI7O4pbHv7AKG6GE2JOh5+cj2N0RjSd/Glj6FoTGlvJh4JEg0ZeLaLZ3kYuoIaMLF9AP9dmsAlXaxQLNgUSxWEJsBzCZgKU7s7qJaL6IUqoc5WSjkbI2LgKTqaIsEuoiCIRoPkCkUMw8DzVRTKFPIWuWKVeMM0OqbGGRudZCI9gV8pkq+oTNU8ImTojoS4/lOnE+uAj3/wObzOKA2mSlDzcQI6dWkLTVHoyTuUFZWYIikjaAnUJpRGSxVGLR9dMfAl+NTcC0KFhro42XwZ165iKAotkQAaGsunt5MgS2rwGFJRqVg+ZUtStGvu2qwniCiS6c1QsQQpC2JSknJqc/M9RagA85ZECBsat9+yirt/9CqvDCts+tNlTLnoYR6761N85JbfM/D6jXzpsT5uu/RU+l2bl/6yGzToGyswtTVEKl0hEmvgngeeEpdcslaesbCFv244wCXLpnAkJ3AUk6PPP0t4ZjcTkw7V8RMsWDWTG94zmy0b9rL2wqX0DTuctSTGn/66mcZ6jZCicOmFq3n8tSN0hsq8clznzif2/sMx6R/OKTbUB/jsTzaRHHwS37Npbmvl5LNW855YmqWL5/GxH7zA8e9t4+JzFzD37F8Sb2vh7eN9JEWUvu2HyLuwdNVyCtV+SkqIaYYka5doro/ypzseZWnbHEarBZbPbEPmJEIILEcleWgfvzwqmN8eY8/WowTVDCeO97Hz0ADLZjfSUjxBT8pl7eI6TlsyjS1bDwE1/6CGJCR9NKGga5KoAtJXEJqPrkIVgZ0HU/PfbQk1hOIRVyXBujDZTIWypxLzNDy/SrkqkIrE8Wqz0rIEjnQoWLUuSzcF0vMp+4JASZJBpckA6fhE27tpnzGVkf0HaJmxmMqRfo729IIwEG6BkmvRNGMFvzuSRGuYwoJFzezY0EOllMNSwKt6mFIlnS/z3nM6+IBWx5lxk+0TWSZKBaZ96os0xJvRDZNZczpIJVNUCgpDfcOAR6FUIaCZ1DU1Mzk2QSgQwBMaxYpLfbwJQ1cJ6jp9vUcxFA1b+oQ1g4Z4hFhTnFi8jlIyR0tDHC3g43sa1bJNUJdIJNmSi1V2SZdsKpUiKAZqQCMSCSKETTGTg0AMr1wmWy2xgAq9EyPUO03EWpqopCZwFYEmPVzPB9shWSwTVEANmAhfYrshFi1fzmP33oURiqHrOpF4AEPksY06PjK/hXysyrZXDzO9w+TJZy7kD9/ZxqsFFaoVppfKTOgBApZDjws3zQyxZbxISDVIVx3mNkry5dqaAwmoCkhP4gsBjotr2xi6Cr6GJiBdcmgISnYd62d2IsCcmYs5dOAgAU3iaALNg7QNDaZgJCdJ9oKmSSwfFFGjco6XBUGtNls/MGSxrEulOprnpV6Pki8J4tEZUPj9Qy8yvx5WXfZH/uXrH+HBR16kONxHeO40DBr46Kn1bBwxmTpVR/Pa6O4IyTVLFzI80ccHz5hKoQSJuEeDXsdwZxvexABfvukKvv7dMTa91UslnSNd8HjmJxs4MGDzg5sWc94ZM9i65wSPH1VZ9/Cj2C60tYU4bdkMIgGNYtX9780pHulP3276pXVJO0BXI2gJlTmRLL99cA+yeSqrV0xB2AbaxG5GciYXXrCcat9xhsseO/b3oCoG+/YdpZAs0BkSPL9jmN7Dfcw0R3h+a5GV8yIsDaeYGBsmqI3x5sH07deuCa478c5+rnj/GgontvHKk6+Q9E2iMsnAOLz93A18/NubueT8BaiFIgePp4iYGscHKzQrYCoQ1AS+CsKDSVuS92utzGhVUHZqamrEhHBAYPkC1/VxFAPFt5BSEBQeOB4tcYHlAZZEkaCbConmGK2zp9HS1UD7jClU0kWCAYdoRGIEFfA84iGfUF2ElddfwZY9PdRN6cDN5RF9k9zU0UKTbzFqu2Q8g/ap9bTM6aZ10XQyIz2kxixsN0/AcXCloFqtoukKc3WNR/efYFfBQlbL7Ml7nLp6DYmQy1uHJtm4dTNHR3JsebuXWd2NxJunUMnmSY6O0t/Tg8jnGDp8kOxQL8VsilMXL2TX7v2EE0F6jw4ALvFgiEgsQiIWZWpLI62Nku6uOLbj4rsKVr6IZ9sII0gp76AKlYrrUXCrCF/Ddh2wLRxcWhvqcK0KmmEQisWxSnl8ReXUU+bz/Mb96E4Z1/OQ0sKtuJQtG0cN06JrlBwP27bJJnOc/6H38veHHsSIxMCzcRyPfNWma0YnR1NlzlvRQEvIpL2ukcjsEIXG07nwXy7grZ++QMO0GKOeSZtdYmNO8KUZAerrIvQWbQwpaI7rFG2HSUehbPkIVa/JdJ6PqutIX2LZFrFoCHyJ5biETYHjgaEKipUCVrqMLwQjWRddQNisiXSWhDoTWhMghWB6XBA0aoBpqoL6UI3XTkkVRdW57vw4tz8+yrXvm0Nc89g5UKGxOsr0KYIjSYXpjPHZa89i4+4hGBumb2CY9VuHOa25gGqlWbW8i2defodj+3czK+Hy+2eOU8rmCOZHueeZt8lN5BkoCZ56aS8JzaGhpYGOFpOhY5MU8jYvvnALv7z/HQ71jPPwhjFevnMKf3thElsYmHaFV3aPiYoj/7u3z4ImXe5oXXjSiu3rz+WNv7zDeV94mqaQ4LR2QfPsdo6Pllg2NcyO3jI//EgLBwfL7ClMx0oN8aUrTqa/VKY4NMLL+xWefu5t/vzH63CJUbYKJJU53P3VdSxflCAqHHYNacxbtZTOTpUlczp55elNPPNcD2efGuPohORHd9zEQ886/OJn1xOvO5Xn13+L++67n7ffSONUSuSSJbp08H1oMyVCE3iOIGpISi5ENIGu1/jBrAUdEYGqgKH5NcZEAdurKdCaJihVfCpe7cXumN/KovedR8vsZcQiGmY0imFEkVYORQ1BtUTJylLc9zTqtLXo43vpSa9gy/GNdCw+j6MH3mDAFlxbsNmZHsdOTqIJheeH0rTNn01i1hRKKY9oY5T04CRCwkj/EaySS87yWDB7CkYwzvA7B4m11HG8Z5I9A28x8Mz9jJ7oYe4F1yDyaQLt8wgGIdY5jc9f9jGOhbrp0qvYdpmjW7YSDKkIFBqDYS5eu5YHNmygLhZiz+Eh4rqPaoZQhY9T9Djj7PmYpo5mCiLhALalEQmb5CbTlKsVfKlgWR6oPkf60+QyaVxP4DgWvhZgakuYZKqA0IK0t9eRTE0QVDUadJu5py/jnnvepC6kEoqG8F0LU5UQiOKULSIRFccVWHaRmS0x9g84VAyFi84/g8nsBHs3bwdHEG+pY2lc46OnJcg7Cmo+T+5wiuyJMqvv/iTb7nqO9HiVeZcuZeLgIU68OcCsZTGePGyzsFVjNAOWU2HHhE9WqkjXQdVMXN9DVxQ810dRJAFTRygKtm2heRDSJaaioUrJqpYG8pOjWJ5FWBdYno9UBNEIWJakWoWJiiBdlRhK7VBbouaKmPDA9SV1rQk+d67kqDMTvVrk9ItXcf+fDvLtfzuHT1x5J/d/pZmL1g2jKCqP/uVjFPYdYaSkorVP5ce33cenP9jOnX8vMHN2A1/61NmMHO5nxuIObvnyY1RFhK9dFkVKhYZoBDOs8d6v7uWxr04nL0Ls7nXJZco88Wo/L/1bN194aIwLF6p8/5kCpy5o5IU7z+Wpdwy+eOuDpB1fVP/Bwss/DBRndE+TppVhaDgv5s+fLo/1D3Dukgjnzw9w+3N5rj+rgVK6zLLTumhXJV97vJ8vntPOnW85DBw8gdR0YhGNnNCZFjU5c6rHO8er+HFBtujT1ZZA5vO858wOXt1TYVGXwfbdw4yOF4lNbWNaXJBOZdB9h837Knz+moU8nlrCF8+3+Pov36DFrPKDr1/DBZ94nE2/P5+bfrSJ4X2DqIqgThNo+MRVgaGD44Kh1oQRXavZC0wdTA00TxCOSIrFGn/kuYKyJWnsiJLomMLMJctp6GigccZcrMIA4boO4lPm41aSuHYRD5NIVEXzJRWriqZbjI1YlIs+r21+jubus0iOD+EbkvLzb3PqzKmkciWOj44waVV4O1ulrqGOeEsjVjhBpFQhOzlEuL6FSj5HJpXBEz6XX7SSV5/fxmQyi+O4HHv9G7z0y8dI+hJVNcikJnGVEObc0zj9nCXU13VQdXUObtvNg8+9xsH9PSimyhxTJaJrmL7N+845hx9tepvGkMmR3lGiQR3PhVhDgu5ZnTTFIwQD0NaSwHN9Bk+M4TgCx7MIhcJYroMvPbIll96+QVQzilXOUsm7KOEAmqzi2z6ZYoWpszvRfEkukyIej1FI5rngwsU8/+YIycFhAqaOpquoqo+q6Bimil0uEQyadMyYzd6DR/ny177JHd/8VxoaIpiReiynilctUQ2E+ft5kkE3hJMV6HoZt7GdSu8EkWlNxLpn0rByKbprM3r4CXb/5AC7fYWAaZCqQrLqY9k+R0tezX6CwPYkQUPFcnx830ERkmgkQqVUIaCpBPBx0EjoPlHNYHmgwmCuBBICJhQtKDvQ3gBBEzJFiSJUgqZPsqigKJJ3khKJYHvF56nvnMxd6yc5d4nO/iNFVs2L89qWPs5bO4/4svP4wc13EA9KZEAjGvQwwzFKtkZAVlnaZVAiwSt7kqxZ2srE8DAZz2TwRIapLSZDaY/FXUG2DHp4ikbUt6kPCY7mfK695CRWdgc5NDjMpz+wkvV/30xbg8qtvzvOh1fXsaAryLcfHiPWFuNYT0qEhf/4qecuufyZF/YKKf8jxrv/LKD4rh9JKBoTe34hL7nwNm745NmcGChzxeo6fvzQLl7c1c/lS6M8tCnHnIWdvPHQh5nz/j/xh6+cxaO7sly1LMGDT77FR770BdK9bxGOhSgYM4kFbIJeBhE0aTQjOEYAx3UYGximqXM1hf7t6O0xDh9x6Yo3YHbECPs+WvoYuUCc5N4+ikY73/3Oj6liULVVHv7VpRwdjnPW9HE+8LXXqE7kSQhJiwZxXcEWkogGgSBIF6QUeH4t5qlcgcYA6KqkZCuoKggpiRqS6SfPZ8olH8GoVmiZ00Xq4CvEW1sxogupn9KGX0gi9SjSzxEOabi+guUoeJUs6ZJCLlNi17YnKBRNWqctIJ0dpmHx6ez56c+Z39JEulgmVcoyYGv05Qo0x+MokQhaQEd6gnImSTgcJ1fMMTKa4aZrT+cvzx2mmpnkh7d/lqBXpTnfw1+eO4hbzTJkaczWVAr1dVy+tp3YWJ7fHsvz1V/cga3oKMUEx576FUeff4UjwQgzVLAqFld/+HqueuRRWiNhjg9NElCgY94c6sIaCJjR3ko5N0l9LIxv21RFBKuSxiWIbVVQNQXH8zg+VsW3KxSqRdxCCUsNIH0JdgnbVsmVKzS3NdAUCTA0PspZS+fy6Bs7aZCSiy9bQ6qscvidfsrpMiXLwlQrSFSmdHTSObOVgxv307ZgBgc3b+Hy669DjJ3guB+i//BBqraD1AOctSDI55Y3UomWSaw4i9Jrb1IUQSrhEIVDOSJ5i8ZrVlHc+g6TwqTcm6RUTpNxfMaGK+Q86Elb9OYdhKajCA1DdWudg+ehCAUhXRRdw1QFui/RPBehh4gYgtMMm5RdoFCuVYm+J4hHJKmCpOoJIiY4UqArPpYPFgqTZcmJKuQ8yc2XTuXtwx5vPH4abaesR4tEqddTXHb+hewbTvONzy0l2T9OXUM7drKXalziqjG08UlKAZPG6afyte/8kR9961pkuQ8xOcHwkT48EUTTdYhrZIYLxOfOx8kVMepj/OXe5znlQ+/hrKVtfODGJ1n3xTW4o0N8+QcbidWprGhWefG4z5+/soBIKMJ37t1HbEqcmy7u5EfPu7z++nbxX55TFO9+xa/ceIb88XceIKvo/Pzm06hkkuw6lmL24rnMCfQxnCkykIQzZijc/PtJrr90KfmxUTJDx/n2r7cyMV6g0PMKlZ4jfOOeA9x6boDv3/ECMjKT8VGfv7+wn43r32T79iEcIjz2+OtoRpgd2wYI1Hdy/wNPEay6jI4e5dCRAqmUy+5jA5iJBuaH02Rp4fu3ns7NX36KjLR5/9VrePa5/RjZIkEhaAgKNAUaowI9ALoQtaA7T1JxJBqCqq9QHwOh1KpI2xUorqT7/HMx568kEojQOquFoBEk0jaXSLQDxfQpYyFa2yERxTMT2LkMqm/haGEqo31EYmEG+rcyNJTEFR6KpiJjcby9PaxpbiJgKOQtm3IZ9k+kUDUDfB27WsKtlrAqFo5nkUlXqeQkrVGVVMVD5Eoc6nsBu1omJJOohuCqn9/D+TfdxCXXXcqyKz7K8T8/TqyY4Q/7xnnf7ASHXvgL2kiWvz37LBd/4RsUXn2C2bEgtq/QX6iwevXJPLl1NwFDYBtR6lvbKIwP09rRQkiXaIF6Vi5tZ3gox8pzTkIzVDJFk7qAjSugmC/QP2khhUSqKrrvUrYFEc3Bsj08XyCEpOp66KpAk1U0odI6pY6WkEbBFux4Z5ywXWLhwnYaOyKcuWYRRv18Tjt5PooZYstLm6jKKu3ds3BxufUr3yI0sxNtaD+BaCuTkxNIHApjQU6aO8Bo+434j/yQ0Pw5KE0GldEykWkhsrpJecsA0dPm0eAMEa7YJEJ1TFUDTJ8boj4M0wyV45aBZfmoqoamqgSkh6fWZr1tzwdfolJbTKYLUHwfC41oQMWrlCCooiJ5d9kkiZCCrkBHHMYKElUImqIwWZB4iqBOgxEbrrtoPm/sTfPkayN4uTQv/O1Wnv7Dm3SvPYmEqdLTK8gWcrz62hGODOfZvadIwW1jy8F+RsbD7Nl1gvLYcXbs7GPPtjH+vP4dOpecxLEJlaMFnV1vHCSdqnD3Q9uIWf3sf30PT+222Lr1OJ9b28rmXcdomncWbe05tr89yPQ6h7kzY0yJ6gQTLWzYephLTm+lJa7wiR/u4+5PNPH4ptTtnu//1wZFEKw+uUvOb6tnRVuVbcdKPPn6QV7ccozzrriJbYd3Eg11MtHfT0eDIBiOseGp27jg6p+wcfsIfSdylHz46hevY83KLm7+/WH+/KMr+OzXnuHKm69gbpPPrGZJWJVkUsOMlmFJo+TYgf10Ta1Hb6ijmE8ypbUe1xogodVRyA3zs3tf4ty1p+CXxuhoDTN09DgP/WU7K1ZO45pVguJYhi2HxlCyZUI66ACKwHIljlvbo+FKiRaGcFjBDKgIX9aM166gVIWoJpl3/XW0TJ9Gc1cb0UScuGnix4JUnRI9T79MYeMG/L4+Sm9uYOTP95G870nCc7oY7B/ELWQ5fOAgO/buYNeWHYhwA8gq+XyVcF09u945xl5X8PjG7aSjAY6OZfAViSoUbKvKyESelpY6Vi+MYVk63d3NLKz30H04cniSTU9+gD9980dkXn6GQGuYxlVr2fv2Vgb2vsXEZIqHPvdZrvrD/Wx+6zjixH6SwSglLUIxM8DiOVP4+4MP0Sp0RooOKApVx2XGrPk8vW8vmqYjXY/sUC9qtIm1a06iuaWDro4QqXSRKa1RJsdLWJZPtVBAaoKJ4SzFso/w/JrKrgjKjiRo6kjNQPU8PFVDcSWW4+A4Hrrqoes6h46NcNZp8xidSFHX2ERbZwt/f+5NThwbZ+eew3R2SJyKzYaX3yIcMqhUfDo6ZzLYP0Dv1hfYt3M/WrQJRwvgOmXy2QqEfBL6HC694b0cLzZQOr4ftSGG7Q4Qqq8HRcGdyCDGRmg++1SCbVOom13CyoyRCAaZPFZl6Q1L2bcryb7+FNGwiaJIhJBUbA9dgKbruNLFkFrtfVIUPOmhS6gPmbSYFVy3ZuWJhsAwBVVb4joS2xN4suZjzVu1SaqCA7oCvTZcf14Hh9IFrloV5W+b02x4/A0u+tSlVPv7SOFyYPs2tuwb49r3dPPM9kluvKiBPaUg08M+oYZG+ndvYm5dgAuu+yCPPrWZy1c3sGD5bJrqQyya28BDT+zj8kuW0e+YbNmb4dcPr+NPj27Cqrj87ImD9IyWqUz0snlTP9VinrIt2N3v8tUbl/HQxlF2Hi5iFdL8bkOB82bCW0d9rn3vknWv7xy4/b8kKEZjMXRNRfgeP7/15HWhgOA3fx+jfyDN4haHOaediY/DfXc/y83XruLI0WFs22JQtpEpVGmoj/PrO67knQ1vkfIUUid6ePbtcQJeleP9wxRcBTMseXPnEM9uHmHvwWM01tVzxtIYv16/H63qUTI0XMVF90uUija/vXcTq89YwiPrd2LiMHPGXNKFEpbvYGVzfPL6Vdz77FF27x7m4o9+ktzwIAcOjdEkAFcQNiRBvab2BTRZUwOrAs+R+K5ENRUUQ0FVBGZE0HHSKvy2DsJNERrqdOriHpW4z+Cfn6H4yPM0zw4jQi2kJjPYVZs577uK5g9dgtk2hWh9G7niCJ4aIJ9L0jlrMf1jY2RSZTKpIU4cTBFurEerpuhaMJ9oIspk/xC2VMgUqgQaA1x53iIWt4Z4bmuKxnaDyjsT/Pi713PTDz/OZ249mzcfOUZXXStK20Kcoz3s3Jtmx98eZiJnseOFV5k7rR0/vYELv/FtJkPNePt30pXQiJUlP3u2h2/e/W/kBvtowMVRAhi2T2LNWp56+WXqYmFKuSKx9lmsPnMNfjlFcqAPMxCgsT5EMFqHXSiQzrmo2Ci2zfBEgVBAw5FQKDsYporlCuLxKJWyg1QVnKoDqkRzJFnbJmGqlGwfU9fp6R9l9oJFzF64kC0bN5KobyIa0qkL6oyNF9FLZQayVfBtZs+dTyQUZDiZZM7CheiKyp79R2iqE4wlixQKJfAlBwfSnCl62Xx0mEfenOTzP3+SyWGdyaqFt+EQKT1MSVdJOFX644vQN73O0MobUA5uw/zo1WjhBdxwy0mkCxZHDo3heiphXTCzvZHjGYu4JtEwcKSLJsS7bJNE9SUNpk7QrWAYsma/cRQCQUksBImIoOQIGoK1zmSiAEITxE1IVmt+xg+eGuWNl8f57Wsj3H7jqezsGeT9i2ZzyCrQZEqyFuQnMhzLKxiKx/NvDzNrdpShY+P4pk//wSEO5yUbX9vB9798Lt++fw9HRipsfWeMo/2j9PRO8MaeIarZEslclTde200uVyL8rlvjyYc/z+8eRSKcxgAAIABJREFUeI3n7zqXb/yxB89TeOJbC7n+u3toiwru/PmnePm1PTj5KqNE+P7PLwTLIehV1g3m9VGkv9Nzvf/8oCiEINHaLFvrYutWTa1bd7QoLr72PYm2Pzy7n+GRCj/4xQex8hlGBkvk97/J3knJWN8gd93+fn58z0H+9aYV3PJv6wmWRrnjD5tACKKq5P67b2HDX18jpUT44bc+QTmVYppWoakhwYyYT0drgjmtBgeOjPPhMxcwfWqcoFtgTXcCv+zRFFXI5gt0RRTWruiiKWjjSBd/vIfh/nFEOcnGbX08dc81fO+Rw/zyY9MIdc3n90+8RaMhaI2CLwWWDY5TC5aVSi0qTCgS1YVUVRBTJdge+srzWPPhU5jaEaWzvQnHhnLYZPTrd7Lo6gU0dFextx4isuJM5nzoarrOvIoJ2UchP87xgy8TDinEo03opk00bFJxoWt6F9WKj2YoHNk/SKChjsJIEt/2mBw8wUSygqd4fPvzp3DLh+bwk59uYqhiIISHM1Hl5WduYdumLex6+m0CnTOI1is0nn4Os89aSMt7P8l9v/guYaOVUDXDlIDKKVefhGYs4ppLb6LYewxj9ck88Ncd7FditASCHHx9G1f/4CbiK+M0zo2zcGmIZ7dV6Os7jkCgmQEq5TJzuhKEI0Ea6urp6G7Go8a7Tozm0Q2FsUyRUtpDD4Kh62SKtVCFUt5CaCZVoFDMUarYGEGNSsGhWqmgaFB2JfWmQNMkigNDI8NkJodobo7R1GCStyWFvMDUJLNnNGJoGu2L5jMxOoZb8FAjQcbHJsHz0II646PjSMeh4ghURUFRXNzmNrq66lAS0xFvfJ+6riDdbc283JPhw3d8k7knv4d1j21gVUMe1lzFmpUr+Nf7t3Hhigg33PA7CNkc68mSHMxQ9l3iQZOxiQzfubCTBw/kiRoS3/MJaApF2yEeDFJyXLqam7HLVTzPrkkBEhQPLA+qFahUBaGgwAgKpC1ob5AcTSkEpUBpjfGrf70SY95qlrUkCVg5ntpnE3LG0GyH4tgI0xtbWLKsm5OaNGY0CBbNbsEsFOhqDmK6LtPaYqzsbmRaayObd/Qzd0YjYXyWd8aJuVmWNulMnd7GjZd38dJLA9z86XPp8EdpiTjsH5OsX78VuyyZtWwmMpWlfYrOxWumc+9fe0mVHX734Js8e1sLLx81+Zf3NnLp5zaQUCye3uWwelr8YicQXmfZzu2u7fx/0M/+n0i4kRJQaJ/aIi+YGqNguxzJ2AhFIV3ycKp5wnaeGy5uJz5/NZnREe7701Ym8h6XntvFB09v557HNjN1SpS/bcnx029cRV9vD6+8tIu3B+C5O85k27Eqe7fu4sZPv4/7HtmG4uZ5fX+JS1aF2XLCojev0qQrzGySHBv3cFyBqakUHJd6TaAqNpVyLc7J12pJN56iEAoLHAuiQR/LVTANlfGyx/ofX84pH3uUC+pr41qRqCCoCRy/5jH0JEgNdKWW8JPKC6K6T7g+zskfOR8RbyOoOihuFXPWSrSXniKyej7qQD/2/uMEzv80gWmdVErDFIvDDI9PkE8V8FubOLZ5D0cG08R0A08xOWV+K4TDTJQ9tr+6Bz0cZeTAYRRDUMpKkq7FXx/6GE7vJH9/cx8nDpZ56cQQV5+3GDed4SOr2khNjlN2Qkyf3oY0LQg307ZiCQ984W7ev+5jTF+0iEce3cXWe3/F6hXd+EmLE2aUDywKsHTdTURtyf1/PsIzd95PzK0SqotwcXeQvoxJrlLAVAVu+zT+duAdYoqOqvh4aoCT157B8hkNaAGBVAKUxkaoqw/z0uuHufiilfzmNy8Si5nMndVJ7+AYo6MZLNsm9z+oe89oy67yXPOZc8WdT86VS1WlikqlHJGEQEICjIRABCNsE4xxwBjjpuGKa7jY+JoLDm1zbWMcyDnKAoEkFEtSKVeuc6pOPmefs/PeK685+8dS9xg9Rv/o4cvtdv/f++x9xl7rW9/8vvd9Xj+lkLPwW20SlZAvFwmaTfwwIRWKnGNT9QNKroMhE0whMbTASxRSaVKt2bZnM8Ko0KytEftd1ts9wm5MvlzETXts33cxLz3/DGMbhymYimbdoxunpGhUIqm4mrGxYS6sFHj6VIs3vnET20dy7HjDJ/js+97EzoP9XH3Hx/nan72XDVcc5CefeYhf/W9v48Evf48TCwW27qsQ1AO6Ky3+9t4j/N47buQLX36ALQMl3vPa81icX+Urh6s4aYxWIRoDS2b30qZCjg2ugegtY0mQFhSLkMagLeg2X1Y1lKEVQC8SeKliwYNrbjufPTsr3HvfEZ480yLpRBBJin2KtdiiEqe0U8WUAWtx5tRakQY5w6Chs/2vK7KNecES6FQzLDRXX27yb08E9JsmA+MGH7h9A1/4wQI/fC7gwT9weMsXBTIIiELNv337vVz/ji/ht9tcOiWYjwr83fs3cMkHjzFuw96dZSJniA/fAG//3BncgSHK+TyW1JgC9g24dBLNd06t35g2Gvf/X2vN/x86RdPiit2T+o5NJY40Q5a6CZaUJEIjtMLKF+kokwv3bOb8nQXe99GfsqlP86d/+GoWphe48rq93HvfUdY9Qd1P+aPfvoLP/OkPkCas+IJ/+ulZPnL7GB/88iITYomfHfd44pRHHGrW2jGraymFICHxY1rVBBWmFJQiDRJGkhQ/SAl8QUkJjBgKSmDHkkmpEZFG9aDWEjRSzdYBRa+nMLdfxh23nM+zDz2Ha4GIM6gsqSaJMmmOYUsMJ3MVYEpyJRjdtBl78y6spEUcJTj9FeSTP6Rv/zbMTh3rwJsJlaL/hptpLBym2W6zuNwkiH0ePjTH9DMh3fo6F28ZZrDPIV7zWGs0WZtZoldtMXN2nb4Nw7RWmuT6Bvj1338Dv/YrO/nBPz/G4VOnWYxKPPDCHM8+8G1+4w8+wGvedgf13DjaPpeTR89S2exy6kyP1TWff/znB3j3u65lbDLH0nQX6ZqMX3sLR797L2E+x+hoif7BAVa+f5gf1wSDj3yX+ZMNrr79Ct5w5RbM/goD/YLBio3Wgu0i5LBn0us00amm1k64667raK63MQoFkk4L0zRohB5RIMhXhvjSP3+TfRceYH11hSRNOHPyFH4Q0WfnoLECRoJMQlKvQ04qTEsw5SjycUyiDCyd0g0TgiAljUMMIRGmYLCYZ3V+FWWZDFQGqK5U6R8aZsPWHbiGizAUXnOFgaktrJ5dxosyHWqUANokFYowSDALkmo9Jql3MfuLXHbAwi7t5cnHHuC+x9tMDMxz9LTiqR+dZNtlE5gdj/0XDvPVf32B3rKPTELGLZuZFY9cGHLp5edy6YTLl+47yoffdR2f//rjjPSXSFKFUgnF4gCkEUNmyrY8xNIDE+yiQOYg1y9RgUaEYNqQdzRJIkgVlAzJvXXNpz56E2/93e9D7LFZaspKMm5pSCX7HEUxhTgWGHEWcaFSmNSCcSNhQsC4SBmPFRUU40mKkyjyqaK5niIDQRwoFtYUYdTlwedDynnJdM/g9KkAaWcYtRGzwSuuuYz1mZO0IwPDNrj8HItNYwUWOykrS112TFgMjpT46QmbgaJLqlJcSyC1oBXH9Nnw2i2Vt58OxD2drv/x/1lkHeOX2SEKIF/Kc+XOMX3JkM1PFj0CBGkKltAYWtBvG6SJpjI4yDcfWCQIffy1NWa6gt+563yqjYTZ6RfZc/EruP6yQR58cp0Ltk/x4KGjtHzBai+Tvjz29BJxJFjxwIlTdqWKPgMIwUAwKCGHZtQRGCJbeMRK0UoFlZcxGK4B46XMgTKWz+gzRQOKlkCj6DMNqk3YsAEe/slhPvThO/nMvzzK3nyKdLNgddsWuGWBtAUqzqIIMDQKcCyL4VdcRNSqUauuo7DIHT3MrttuJF2aRtVLhCObKOw6l3DlLIvVFer1JipX4tMf/SI3XDDFzTdvQSnF5JBFcz1iZFJw8JwhhJSUhgts3DLMyukqjRDM0OPEA0/x+ENnKW4+n0JUYvnUHN/8/DvotU7w5F/9V9aOP0LcPE5lOM81117Px77xJL1Ts2jT4dIRweiQRFuTvPv3/pZP/fUnOHfnVm56/8f43J9+DBUnHJ/1+dlKnYOHfsLK5p1sHarwyrdcQbfnQZJlNzeXmqRSkCQxsWlytt5FBh6vfsttlMwccwsrIExsndAh4ZFHpjlwYBdf+PvvMjY1weLiMptGR3n8yaeYGikymE8wgBEzwgtjHNOiiEnRkqAkrkjZODXG+68Z55kTy2zJ2RTNhGHHomRalE2T6XqXYsklatRpel2sfBl6VeaWazTaDTrdgE6QYiQtxjdO4HV7BDGkSpHPm+ggws7lEFJijZa45UKb519KEaWAwWGTFx57np5pcujHx7n+pq3c/+g8ug7FKYfqTMB/+uQt/PXnD6EDxalGj/5CnqYXISsGjx86SaGvQN62yY8N4a130IYJKLp+RN41WI0txgsmRtKBVKKFIorAzpn0VlJKlYzmrgXYUjA2KDASxYW/ej2Hf/4kL821GQ8E6x3BQF6zvQSGpdGJoBHCRf0wG8DuPugJyVAR4kRjS816D4YcKDqavCkZyWtme4JuDBOGoJNAn9IcbyqmfIEZwwvzMZaWtBUUHcGJeZ83XNbH5340yw//7Dz+/BuLvOetV3P48eeIE4ujixFnGimPzdhsHswRRyFbSiZaCdqpQmhBM1TMBwl3bynyonLuUX748eR/wnb6l3p8dof6v/6Zg2N3PNvymelompGiHWs25AXzXcVrRx2+s57QZ0InUmzasJWZxWVG2qeZ2LWNV18xzOd+vMTdr7oQo/0in/rKPC898BFuuu0eXDQmil8sZAHzaCiLDLNVermQBbHOYgEQ2KbGQNBKdBYmLsAyMtxXO8rI1ZahMbQEkVGyZQrS1AgEXprh38MUEleycUry9Jzkn776QT7w5v/GBX0+0rSy5CAiDGESeTFBmJIzYKmlufyaPTQGRxCmgds3yvpik7v3RCQD44QPP01469tQ5U1YsksvzvP8o4/ivXSM0/Ue73/3pXzvoePsGLeQKyFPd3xuvWCEJ59ZY2a9xoF9E/hLxxi/5G42Tji447sxnTxxqJGWgx8EJNJleW4e677P89RMk2t3DuNsHOfh793HFa+9nfL2XZjddaobLuCv77qbG1+1n92DkmdfqHPTqzZy6vgKieznh8/N8aEPvZNP/Ke/YqJfQCIJY5+ldsr1t17M6645h7XVJl61gRVF1M426UYRXigRluLZaptHZtv85h+8i/mnnqbrlti7bYJYahZWlmnWDFbPnmZpoYZCcc7IEGdXF+nvExwsKkIVkaRFVhsdpM5TKuapCI+2NumSZ0/DZPIGhwtvOcDdv/Z9+i3YOaBwSxOk3VWe77o4dsBi4lAPA/osjc73YVoCmUjylWGi1gr1IEArgzSJKZZzCAFxHOL7mv6cjZE3aDZ6DGzazafvTvijT7d4z60u7tR+lk7N85X7jjE4UOC/fPQ83vfuhxmcyrGxkEM4Bm9+yz7cksX73vwV7rqwj8/P5giiiNvOG8CJepxDyENhnuJAP3OLDdaaAd0wQACu5eJ5XSYGBriyb4lAprTbYFoa1YXIgIorMAxFpwuuKzA9zZGJnZxcmqe16nP7GKz7moWGYNtQ5qnvBNCfEzgGLHYUm/skHQ+8WDHfFkwWYbICTSWwDc1aAwouGCLTRXYCTSfOolfNGJbCrKJEIssql0LQMbLPUFJx4eXnkCtt4PzcC3zwX1t0j7yHc676Iu+41OTT9/e4/LLrmJ05jmtqpMiYo3vzmpOexDYVnQRypqZiSm4bsXkulnzhsbOCJPqP2CkKBkcH9Jt2juxZ8kOWPE3V15hCQQpSSsqG5rG2pl8qglRTGhji3P3no9OAU2seGyYLBIVNvPHKQT77xUd4/JFF7nzfO/nCp75ItdOjYMGJNUFZC/ICNhiCgpmlmBUt6EWQN8Ejmw96ERQtjauzHz39P2YQJriWpuTAcF5imdn7KiVBLidxLRPHlDimxFACy1GkMdRXEsbNlK994ReMn7eD5lIVM9VoLyLoKkSSkoSaJM2ewGZqkN8wiDG6hSQNaVQ7DBgpw2tVnvjSg7g7LqC3/yrS5gq1TsIzDz5ENWzS6axz06UV/vJbz3LHtRs4dbTGYq/NcN7mx08ts2dHjomhPK31mKt+639j89atOIPbScKYsBsSBh6dZpNT8w0cw6AyOMzp4l5G18/S6PRozc5R3DFKf7PK4aUqF7/rnUxZLQantvLNL/2YA1MlxreU+PrhDgOGwZFWjzt3jvOX/3gvXddhuekTSIeV2MBxXR5++AQ3X74J3zfoNTtorXh+uoESEjNNqLU7NIKEc887j6Xjx+lokw0bJrDzDtX1OvMzTeZPHCPodUkCH1CouEc+rzgw5qJ7bVAabbjs3TlMoWAxPKCYmHA5s9ihJop88suvxH92nvzuDWw5HfLb79rGKz74W+w/6LL3vHFuHUo5M91EG1k+ynoERZFg5ErEYUKrU8W0bMYGh1BhD18IjDCmv1Ig9ALyeYvhXdsRXkgUe/T3lXjocMxF4z0aLZszZ9bZtmOS+35whEv2D7Pi5jDP1FlZC+i4EheDX/zsFJfdfB13vPtavv+zZ3nnVs3DcxrbEKgUbn5dmaMvxcRpyqD2iCwTnWjaYYRl50kNyKuI/pFJOkvrWHnQMYgc5EQWlCaExDA0eUsyS4mK9rhOdLh0XGC40OsJNlZgx1RGci9YsO7BBVtMdo3DTFUzUIQkkZw7AqUCuHmo2OCobKu9fxL8CDSSTpwxAIgFmNBIMpp2AQiUoCjANAQjoxCHmtZindfdeR2f/+EC/nqdWy8b4oHDNV553Qg/flJw/r7tFAYG6XY80qBDgkEsJevdkL6cRRBlY7hAwXqSMuRa9PcV71loBx9P4+Q/VlG0XYfrd0zcUzQ0rSBlyU/IGZn9zS3YeH7IoGOA0vhhyM59F2FaBqePPM/unXsZHhnm9a/ezw+/dR8f+J1b+eRnf8rlBzdzdmmJlY7H+371Cr71s2mSSFAUGgeBNAQ2ijARSJ3N8SxDULIyxHl/ThAmGXZRGxklOyTDK3mpxDYUlsy8ylgCqQ3CyKBPJqRpSjknKRYEOtQYUlFyshiAiZJJuV4jZxikaUqYCEwkXiwwbYFpQOpLxHCFdmGCbqDwF1+iWN7O9oGELdfuwi9NIl/zFsI0xBrI88LPHmY5TlAnjjMxYPPoi7PcefVu7n9shbpXI9T9zK+u8dZXTHFiepn+c36FV7zzvdh+lRQbr9skjGFxdZ2FlmJ5XeGomK/83T/x/R//lO7aHOuF7YwuPEefqdjrmPzdnMGdhTWsZpX3/q//nU1HnyIqjXB0bpXxIOHqzbAgHKzZNb51pseqStmkDEYmJjg7v8TGJGCbI+jXEQcuPofUSUjDGK9Rp9sIEaaFoRJWo4SuMmjV2ygiGkvrGAWHQqmfJw8fo9cKmasuUdIgpaCXJJQKko35ADtI8Q2LUrnCBef0c3zJo9+xKIiYtRPLtOM+/vWFJ5j9yafwemUqO8ZoPrjEwK/fRPfMNKnZj871E2wc4dY9Bj/9RQ1lxKz6WSei0RTKFn47pNb2aXSa9A9U2D5QZHSkiCcshKXpGyzD2hrLaz1yRZM9B/fxnX97is9et4EvHq1RTCw27+rjlQMe33uwyQU372a8t87kuRWOH+lgGgk52+Zb//QgRtnlnfe8ibORyzun1vi7J3ym3JCdgy6OGeH1fPbvG4Kex4m2wEgTCgU3E657Aa1uzP5JSdCKSTTYGrAEhpktXYQpCTVMVQKsNKZpaaJEYBqCySlBsQD1rkDGmq0D2ftkqvjxC5p6IjDQTPZrWonkTF0S+Zr+Eiy2YNMgKENjGpI41RTzGcIsjDUxcF4f1CNBV2da3pbOgtbma4pXXL+B2Z7DF7/xGLddtpFfHFtFdTy27trKH//tS7zxrteio5il+VkmN0wizQLdxioSyZaiwbKfUszZeGFK0QYv1hQNmMq5tC37nrX11sf/Qx2fHcehr6+oE2CsUiKvY6ROSYXAVZAzJYvKpNVsceXBi4gMg3qtjmlKup0OE5MbOXP6LEPlNn/2nvO5/F0/Yu+wYGSyn3vuPoebP3wY3UsoSEmfmYEVfJUVv6LMcnBHDU3RytrEvJ3NVYSZ5eP2ehopNI4hWOxkMaFSAFJTyoMfSgqOg2WEtHoKlWbymnwJokgTC4GlBYkyUGmGaTJEQi82CH1FGGcJfK4URCgcCX3X30oQL9KXCIauuI7Zhw/zpn0uQ5dN8OiPT1C66dfpBPO0T07z0AMvUJZtDLuAk65hpUXifBGZtphd9rli/xB9pSKzywuc+/o/4aJz+4l8SbG/TKuxTrXhM7dUo7Fc55Gvf56T6wFX7Rnjbb9yGebSHJZT4MnTLY4vN2n1Orx1o4051sc/PbXKZB4OujHVZsjXZiPOVuu0DbhweBhXx3RNh37bZNA0eN1tOzh4+3XYIiVqNHng4Rl2b++nMVsl7DRZX/Fo1n1UmBBqCWHIih9QCyyCJMTNuySRx/ieizgzM4ufhkRNj54J5W6PrhSYtknZTdjkxBQrgwwYMJmXOFtcVJjgLmjOvylH3HcVzrl7SFd+zsoXTjJ5cADj/K2ogkPzq48w9Jbr6C31SMMILQSN6Rn6RJ7PfvkFVrTi8dWQkYKFlSswMeAwu1wjTk0QKZWijWVJnFyeoNXEtYos1psUSxXu/PXbOfXzx/j5S3Mc/cAQv//lBokt2HfVBRzQx/juTwPqA2UumlRceZ7L+/9ymcGiSSo0JaWY8QVpJ2H/QImVoZRfKQd8fdrg1y/K4Q4oTs35nK4a7N6iefhIwpm6T80zuXxjHyfOzuG6OTaP5Cm0lxGmxClI8gWbOA1wpCJLhHWITZtKSeOUbXrVOiIBlYqMZhIIhAn9pcxabEto9DKLaicWDBU1SoksT0iakKQIkWKZoEJBmmbjpk7HBK0wDE0UaJYaglYiEErTVAJJlkbZBeYV/O7rR7j0vBF+709fIl/RXHtwF08uRUyO7MAPfdZr6ziOg5vLMTw4xOLZGWaXqxRckwlTsxak2DagJNqwWE8EzU7zsGtaF66urItf1t7ll9IppmmKk3P3SJ0e7fTCvyhV+m4NtUEl51Jrd5lvdlDtDu94420cnV3GtQQL80vMzy/g5PJorRgYHmCpFiLCNtfvNTg10+OafXn+4hsLLNcSCqZkW1HQJzQVNwMyDNlQQjMgNYM5SGLQAiQCFWkQApFoCmaWjKbInqZpmHX8QSoQKSRakmiDditmuABaZTOXNIRuKNAa2l5mKXPMNPNvC4mhNX6SiWylhkgKUJrCUB6nNITtjHDHRz6E4W5hf/gDHny0heUFJFsuIS5L/NnTVJe7rBw7wsBoia2yzXTH4PxNI7ywsIhr2mwZssgVh3jwuWVu/NWPMDkAqeeDEJw69RK91OXQvfdy5P4HeXz6CCiT337dNl5/ywGSTkr/watIAsWeSwYZHJhkY9rkC8d6PH+qwRUlHyc1ebGtOd2FgTDhv/7lb3P3Lft47vETnOikbHMVw5ZCeiGvvOtCVo7NM320htAJIw7UXzpO0KizturTqntEno8WFt1E0Qp84ljgp5pUG0RhiOX2sbp4BpIIkcYowE5iXDtFmJC3M6/3mi+YroZ0rJhN/QKhTLxY0lmMWKpuYvLWSxHzZ+CFJXTQxrBigjN1ZLVG8c63sP4338HojyGGOInQRFRn1tiYVySJZKEbEyRgqJhmN2RyahypfFy7SM4xUUKTtDO02nJnnauvvZIrr9nPcw8e4kcPneCr7xrj9JzH3rLgm3OSzQMWaS/gggsLnH6iysi4gSkNcq2U9V4EYUozgFdPwWsuLbJ3j8Pdl7k0TsXc39GcV45ReQfHhVo9QaOIOwm1xGC567FrywTVVshYyWGt7qG0gRmHGJbAlClGquj4Aj81EDIh7QXoJCWq+6j45XuD7IitTRAJhDHECpo+1H1NEGVxGJEhiE2NWZIIX+EOZoVCGZJUCzAEcSgYGM3AJ4YNhpVl8AyXoJYKymi0K/BSgalh0oKfnPK47eoh1ms+l2w1+cZjdSY376PVaiKEZmlxhbW1NWzTZqCvhBaS0aLDwuwiC80upXIJ0y3QjqGdaKor68K1rBu6PX9vHCX/8bbPSZx+A62/MTXsPl1dXSWfyzM6MsArLjqXa3ZsZPPWjQwPVqg2OjTaHeaWltl2zjksLy4xPj6C1+2wd/tW/vs3nuKWnfDkYoqQDoeOdDANyYGKYFynlCwoS00SQVkLCu7LR3ipUVpiKija4OayDGYvzp4srqsxtSRJNA0tEEowmNdIqSm5BpvGXVIVEWhBpZBFkq7EkmoAOSko2JAzNAUhURJGyhkEoqMEEZqKI6jYgnJOoo1hDr7m1Vzz9tvA62Ed/hyVXIVn5jss9xSjFx+gs7ZGqDX15+dYrJ+l2wmQ0sJG8uLsKqPDg4zainauyNx8lbve+17ylk/RNFlv1Fms1bDyw3z9bz9PQ1ocnpmG6ir7JiwGVpYJwy6P/fRJOrNH2XHrJSzet0RpqsLu687lnQeH2DBa4W+eWOXZuRZDSUgp7HG6G/GGN4zROtPk9ssm2e8Kql5MRVq8/e1XEQZdiBKaKyvYYZfO9CJ1P6BWi8nrhKX1kFZi4CcaP/DRiSaSJn6sQEKcChISoihCKUUSxyhMLhuIyBfypGmMRjBZlLx9/wC/c9MuRgPBvcd9groPs00mdu3hqs/cTvqD72MODmIduAhj7QRBvYfUKVEcIRJN/tKdrD2/gnf8LEEYo8KAJEzpemDoiNXUZL4RUBQS07ZotppURibxoy5ez6fXjQhUwNB4Px/4vf/C7IuP88D9z3JmZpltIzmu2QKNasrOHSW++ZjH9u0VCj2kL20FAAAgAElEQVSfASl5cTHi3J1F4lhzYs7HMgTbthY5b8DivrNw9XYTs2jQaadceLDACw+ssHezSb2XEKWSC7aWWV/3WPI1jVAQC4ted509QYOakAyVcoSlCn12j4GSwjIUOhWsNzTdVrZY7Hgiw4pFgjSCXhPyaITOZGhCazSSridJkNi2zMILlCaJs0LmtzRKQKMBXQ90pJEGJCmUBgTKECAUOspieE0L1pqQCs2YI2gE2Wslgp7SEMM3n1hn03ge14RDqwUu2rkRz49ptdskiWJwsJ9qtU4uZ7BhqJ89mzeybXKIi/eeQ7Gvn/VuQHW9jqU6XLxz6J7ZNe+2TqfX+GXGshr/o6fv//OrmDn+5nev17fsr/CdJ+aYW2nS7XhINFGcsGPrRl48dpK+gT7Ozq2wffskbpJy4UUXcOjwc/RVSiTaZMvWKZ4+08UO25yY9ZFKYAvY5cJwDqIEDDsDsmotSLTCMg3iJNs65/IZPy5MNDoVdMLsh0siQaDBjwQOWTeYCEE5D4Ey6IUmoR+SR1NtQ5xKHKAI5HKZCyKf06wHEpVC24d8XtPsQcGRlKQmTjQ5s8Cr//MHEeUufYUSXjciPv0zFr1x/uwrv+DaN9+ObboYeDRXazz+k8fxUQwULBa7knya0DINLslL1m2J7jZ5wwc/haW7lJ0inTDg8KlFuh3BCz+7l1arhqyv8qPfvwBrcIDeiWlO91IcKRnIaXIbh6l9+0mcveOMn3sesZAsnp5hZXaZV20s86pJhwVPM7ZplLvv2Eu3HkMQMz+/xokTNW6+eTt7rt1N2+/iN3tIARVL0Vxt8PCLq9g6U/eeXInw04z1J5QmkRIv1RSlRKJY85MMnRYndHtdhAaFoBNpXrvdohWENJWJI1LOL0OQRiytrTF51WZuf8tFbL/6Zva/5SaGNvgsfvJz5EYt0pMvIb02asd+dFmD18EaKtCdb5Oa4MgCdhlsG5zIwDUSklqHgqFwlMFykLISKpxAIYSi12qSLxawi8NMnLufq254A27/Bv75z/+E2cUV0kRTMCX1KOTOIc3JrqBUsthhpkxbLuNpgFnK0fRTio6k00np1FpcunuCsXKHS68Z4MYdiuef6zI1mafZNHj8JZM7r+pjodPN4k57Cu3Bo42YcaFYjRwS02FurcGNW0q8WI0Rtsl55RXmvUGGCl1cIci5GXS27guaTYGhNSNFgZFocoUs5CwSkqKZnTErRUG1kd0Pcfhy1+hrMCQy1YQ+WFY2emp0s1iNFEEUk8UppIJESRQCOycplsFxM5TZiC35+UIm+K6Rndj6jKxWmBFMDQu+fqzAnbe+klOnT2NLScML2Ld1knLOxslZxGFCEnm86rJ9/ODnh9CG4NjcEs8dPU2r06PZjbnxgmHeeOXG3zHyhXtOztY+LoT4D1AUhaBgS/7wd96sP3jT2D0btpZ4/UfuZ0suQ+nXvZCyKRgb7Ke3VuWt738Hv3jwGdqRz6npZT7xsQ/z6NPPUcw72JZD4PcoVvpYqkUM5v2Xk/MEJQtipclrKOQ1lpQ0fGhGECQZjixngynBVJCzYbAsULGgi6aQEwyXBDYaoSUDJUF/BUwBvUSgpUFBarSKM3inlV04wxXor2jSJBs0Sw2j/SaeklimJDUdSFLyDlhWVmjFwATjl25g2/bLMctjeIf+kSs37OHX/uHnXL9jlN7iGn5/HsNyUcunePCZE5RKJRxTozQ0tGLzQI7ZeoOxXXvYfcNbMbvz5O2E586usXCmSiLytKZP8/BzL7Kts8bbrt/IqlUkf/gUP1z0aRSH6ASKmu8hW5p22GFocCN4q9R+/iOOLQsGKzDbiCkN2xRzmrHNA5DLIdda6LOrnJldQQzmKOdjup0UQ2WLlLDdpnF2mYgsga7WilnpahKtaAQQJCmxjugTgiHHJEliAgzCVBPGKUGi8BNNmgqCNMXULm983W6Wzqwz54dszpn05XMUCzZ23iZdb7J47AzWwhHCpZO0j0yTt2ysiw5gbtyNLvqoKCZSeayhQRLPw4h82tNtnO19yFAxM6cIZQ9f2AxOlXAVRGGb51suMg1oqEyOZUmJ3/XpNtdZPXGaM089ysqxFzBcC9d2EEohDEkvMBiagvMmXJqxywYr4MR8zKCtSAoGJVvQbYes+Cmrs21ufdeNrBydoXbWZ9032bXRJIxM7DShnIsQSYxparqeJtImQZSQTxQvNTXtRBNqC1TMZjulXLQ5ttymJ0fZVQkoyRAjya79xANPQJIKio5AGFAuCLTK0iGLVubXr3uw0gMTTRxnMW6G0qhUYCRQGdLkypI40ay2BJ0eOEJm90vv5Xm8EMhUEfQ03Q5YebALEKUGna5gwNa0omyc5UhYjbP55RyatdjlussvZr22Rpom5EtFgl6LCw8c5LzzDvCN7/0I14APffB9PHj/E2yb6OfUwirHzyzQDSK2l7NG6ZFjdc7dPsIbL9vBhbuG73nx9NI9nUB9/H90VWL8++uhxEBz4c4hfdWejTTlOm/6vXvZmEvZu7XEK/dWODTjsdDsoVXAjo0T/O3ffYm/+NOPce/9j1IpFbj3oV/QbrbIFUuMDPVRXa2yXF3l0/d8jH/5t0OUw5B+keAITZ8tcEyylLUIDJ0tNgqmoJjPhsWuA80YdGLQDRW2C60uGCboUOOFglIJigXwewIlJI6tObGW0vITRgoS14AwEuSc7O+2exLLkrimJsUisS10GNGOBW6qSLQiScC1Ib91P6/6s49yzsRO4qRLGidYT/8j0yrPZ7/1LJ/81auZr3dRpTy58WFCL+XY82ewrRQfC2EJxvJGdoMWS1Qmz2XTplFWqmusdwrUGy3iTo8jL55g9tjzVLwaowMW7plllvIKN47x622W/JggiLlq7xTPn55ncsjGrM4RrVR5/EyDxK9R0JKyKQg6ASuxQWtmke6ZFeaayzxe1ZzxEiJHsikISeOUoBPgLTWYnu1xthFQDzVLzZjVyKDnxTSCNPMMmwbDpiSSkhUfGlFMN0zpJClKacIkJUwl2DZBqtk6XOLaN97AyjNHCKXFiAumFAy4JkgQeYtiURK7FkkRBA4itXHKGu10EH6O3nOzJJEi9E2ckX6S5hppaNOrxdz/+Az1XofN50gSPyBo9Fizchgdn44QVDsaW0h8oYlVgrIsbFMwNWiRmJLYypPGGksoUl6WnFjQraXsGTfoKZPULLB4tkb/ZIG4kbloKkLj9iKO1hU3vuEgOZ2j1a2xbZNN2Etp1RWVHIjEoxvE9KIYP5ZEXkLDNzldS2nEgnakSVWCZdsUi1MUWnOYQrPLbVMZ34AXS2pnPFIP7Dws1AVCw0BeEStJkGS7ldDPyE7djmahBe1YYJPJbvLFLPUvSgXlPkWvKVFo1Mu6wI2DgrwLvTADJRedzGYoVEaW90IoGII0ACMEqTQ5UxMGgiFHsqOYBax1E0FlcoR955/PLa+8ni997Rts3TSFaZhUyhV+8dijvHTsBBvHh9m7eyd9OuXI8aMsNTo8duQUPT9CStg84jDcbzDuwtceWyKMOniJzzuv38J3Hl/6/6Aoiox3LtFUctxww4Gpt1+6x+CujzzKRcOa3eeMcddV/XzvUI22r7hoa4EnplsULdi5bRthvU3XD7DzNpado69cQcUBC/PzXHDx5fzzX/0J3/7u97h4/26ma8vkVuqM5jOqddkVVPIaS2fLj4lBKBQhSTQzNUnehOE+Gy/VOLZJom1cUvIumEYGeo0jQeBrjLxBkICdyzZu7stCcGFmF0cQa+IQ3JzEizRRLHAN0HGEEJKypWiGmnIOSCEMITc6zJ5LDtLtVCmVxph+8mcMJjF//1SDbQNlrrhgO6dOnqUap4jCBprLNU4dPUbOtVCpJgpD3FwOTEG0lrLh4NUUjRbTZ5vMnV3BNmyWaglnjz3DVtWlJw3O9kxUn8OTJyKeX23QTgS9UDFWMJmebbKjT2DYOWoJaKFpm3n6ci5pIrFIOdsKaazXqCcOhxZ7lPoq6DjkRA1+48pBFqshnU7AyXrIiUZMJ0zpJppqB1a7CVGQEqYaC02YatI0YTVMqAYpQZIgNHSjmG4sCaMIZRhIqdFhBJbJtokxrnvTa2h7Lez5M0SpZsvm3aThKqWyQ6/qMTCSQ4xKrNjDLU9hT47BwhpyLsK4wKDxsyp2TmC7LRYfnoGCgfJjGn4f2ystOnFKruQQNWJsA+an23x/WbPWTMlbksTQJEpgu4LxYp49A7BvQHH9ZodXTSnORgarbUXRkcRK4RQLbB4bYHa1SUUELNQ9ekmWrLdquCzWA2baipoHK6st7nj9AfL9Fs1TSxxZSPAbASsthddaISZmtZsiAk21q3E9qAawEBnUvIRQCpCSvKGgtU5Oh4xWsuNyGtcoFyMGBhWWK6i2MrXERFkTxwLH0jhCIhVoW+MHkAhQqSRnaHJONoYKQ4kWgrKrMKTAcrJIjXaY5fmGKgvZKtqCfAW0kblWcoXstOWFMFfPsqeFAblctqisFARKg21qRvKSjg/j117Jlfv30F92+fNP/jH/8NVvIxIPpWBsqI9KqUC73eFDd72Bb/3kpygFL8zMslTzGCubuKbJ6nrIu2/bRbkC5w8qvvp4nTCGazbAozP+PY5Kcr4S9/+/2imaaLaMuFqH6dtfd904d3/6GcxUceN5FfoHHFKtGStI1lsBbz2Q56HpgNPVNpWc5OiJ07zmtbfype/8kM0bJmjWG7z1zXfysU/+MSXt883vfY9N46MUlMfG8y7m5P2/wLCzolXJCyyVBcsX3Mwo3g4NXFMiTU0uZxOjMFNFO9TkRJJhliLIOxBFgqFcZsGLUsjZmjjNHC4tD4TKgA62yJ6GQmTbubyjESpbMduGoBcK5MsPhxBBzsyOzwMjQ0xcfIDJ8Y3MdRW9B79Jrljm419+gvfcuJlue5pmLcdiajFYiJiZnqFbrxNr0ErhWiamaZFEKZWd17FzKkbYgiTJ0aq1WFpco6cNguUTjDiSy4ZyVAomc4HmVNVjaSmkE6dM9juM5W2OrcWYtqQVBDhIqklMWWvqkWa95tNMJWGvSyswWO0E+IngWDXgmst2ctmEID/pEKwmqE6HauTQ6MUstUIaiaDrBfTiBFdDLYF2GBFphZckKGEQh1lR7KWSVDoESQRKY6LRQmGZFn4YccE5I+y+8hKGNk1ibdjEWG6MpWcfotBfwLQMBvokCSZmV5Ab20OupFCpIpqSBJ0eZuRQvHIr0gnwn1mmG/fw+88has6ilUUvaOAHilEroouBry2kbzHjCYIoppQzMGxYaafZ95MxBUPQTl1Sw6Ydwu6xIvOeoGQaJIUSliXodpoMmg5P1GLKokPesKh5HuvrPWSaEsWQ6JioCLvTlLabx2wtEfR6NDoxw0WPhpbEvo0WEIQpCz2HeU9SNCP29Rk8XEsRSmDbko0Vg5xSyJ7PdEPQ9jQVsmOq3xE0OpogycTU8uXZeqozP6u0QClBzgFbCBIgTiHVgiASaCMrBKkSRJGgE0EnEhgGREG2O9Aqi00VPrgCCgLMBAw0HQTNLvS7ZA2IA32ORKXZnDOPJEbhF/r4zd//LXrVZbww5tChx/nQ+9/NbXfcRW36KM+fmMO1JOPjk7x48jjt9QbPTS8ws9pAaLh0Zx+mW+S2CwzyAyMQB8RumVHZ4qGTPrfdcYClIzUcV125qS+9Z7krPv7vkenI/6cvtGybjWMjulx2ee+rp/Su7SO89x37eOJYk6JIedslgi8/3WU0B6tVjzPtkPm2YPvmHDfuHyBWmkNH5xkbGeRr//pVHnniAfxGk29999u89nWvY+bJh3np+EnGhgZZPjtNNDzF33/0k5i2xNYCKTTdQOHpDPff8wRtT2BJCGINscTQMbEfY0gYzCm8GJKXu8pWD7qJ5khDUA8FMtWkCrQpKdqSzYVMiJrEmkZPU/cycKxMsrlLqjRCQpgIBBotNW4hcxNYgCM1S0aZoclN1BvrrJ1+HiP2aCXZnFOaHo12ib6CpLa2wIvPn8VyCwgMQKPIZkktP6DTsbjlFS4D44PU25L6+gpeFFGtLrM29wJWqmlECcdaHhtLHheWUz5ytc17Xn8et17Uz5Yxl+dXPCyRMtuIWaylNJo+a+uSE/UA1RM0OxHzK2vMtCxe6KTUEslc22Mgr4lba+y+fC/t+Q4936OuHdZabXpRStMwaXgBXW3iJykLaYqfRPS0opsIuomm4cW005QeJmGSEQbKQlCwbSwDbC2RKsEwDPJuCRXEdNsx0YkGzUe+gzUxDElM0u5Rbxgklok7UkLUT+H2j9G/91YssQmjPEJUzNN55CjLT66wFvgEWNz/81N4cYVO7SxP1yWTZYtvHFP0lkLq812eXG8SqwhDafJOymBfjjSOEJGm58NMI6LlB5xthJzuwqGlgEkn5sRqiw0FMNp19lx7MycTm1GrxDPVPDPtgHVP46WSxVqX00vrtLsxxTimL6ewGmfRSnLR1jIjrstjpzTPLtqs9TTNluZEy2bBF1SjlEPtAv9wNiVvSvIFA8cImShbWFqyGmm2lDR7R8lkLDXwE/ATgaUESaoZ7BOUixonp1GJQobZgTJQmsjNHvKVvEamGSfAJHPKpQn0QoWMYSwPOZ2hyTohOI5GA6WyRlqQOtmyMVECQsGQkxXKXjfLIap3NEkMBanx0swoMdxq8pt/8Ck27tlNY3mBkdFhfnLvvQyYCX/0iT/mM5/8KGYY8L/8wXuYOT1LrdPj1GqDgxsK9JUc9mzO89qL+7jvBJyZPk2KwwunVplpCj58vc3b//BBPvGfX818V/K6K/Yw1lfQk6ND+t/ZKQqkEPT1VW6Io3Dm/y4r5twtU3rPiEWMfc9zC3BiyeOZZ+ocn69zwYjm+0dhqCh43XkmH/9+neXVkNdfuxXlCH7w5Dq/f3mJR+ZDlqttLtu1kRce+gV//7V/wQpqfOdL/0Jq2YSNFt1Olyvf9mt88Ffezg2DEtOBARdKBUmaQt4SoASlfCYiTdHkXEBpxMuuiFZPI5CkZMJrhwy6mURQsiBvQSqh4wvyOUXbBz/I/mmVQMnJYBE1H6TOtIdCCoIemYCVLFCn3RU0fYUhILfrcn7jY+/A60VoNGvPPkba9PEFHDqxyKVbhukGgunZZZYCC2kZhL4HoUcUvYwe0wrLsBndu5ttO/bQjaDaSKkFIctHT+HHCTrWhD0PX1sINKd7sK5gPYhRrdOsepLn5mIu3BSRaptqOyRMDGqRIo5jgiBl1Y/paYGHxYofUZIG860Ov337Zbznw2+gZDT54ZcPU67082y1SxolrEeSRpxS6wVggpdECMsk1gKFQCpFIgXJyzgnpRLsl+VRRcvFNCFOE4RpZDMypwRGnsE+uOS1NzH/b4+y79f2sfLAo4yOjjC0ZZzY7aMymSNZXiNqakShgHd8Bt/oYrljRF4VKVyUiEnOLhJrQULEhmHNC2sx7dSiXW9RKPbz+GKDp9sGK4Fizk9JovR/J+09wyyryvTv31o7nHxOncpVXdXVmaYT3U1DQwtDEkSigAIGRsw6qOCMOgaCgooZZRxlVFAQRFAJQpNzappuOseq7q7uyuHkvON6P+ySwZn/69/rfT+c+lDXderUtffZaz3ree77dxPSBW1NPgcydTLlYErtKg+hTKoNj6rlU642yFY9MraH7vgk003EzSgxUzGcy3HSrDZeGMpSqXnsK7nsmahTl1GOOf4Y3t2r87mvnsWO5/cR6mhCWHX6d41Tr8CCBa1UyzZbCw5TVpSsJaj4Oq4NFj4KhTAVmnI5uifF7LktbB+wWdVcQupBdrTQtaBPqwQIcIUipkmE5uPqEuGD62nUVFD/KEcScnyEJkBJ4q2SfF4RTQgiEVBSEIsICAW22YobHH+bI4pSI3CIZaqStnZJc0IRj0PVg2pDYEoFmiBkCGwnaCspFRy1U0lBtRpYDdc0itz66j6+/q1ree2hh1ixZCF7Dh6kcbifdReczyUXn8/Hz3svzelmntt1EE2DBZ0mszviNKo1Tl6zgOd3TjN0qMDqY1o5ZvESHnn2EFM1aI3q3Hz3IVqb07xyoExPS5T5zRGmXLHUqjX++L8KPV0nmUjMa1hW/m9PwgTTp1nNSbX3gZ9z3T2PE6lXyGQn2HNgmIlMnikZ/eMp7SabMmXihomm+aTiCUxNw0gsZM/YOK0tJRY3N7jtRYeXf3Isp31pJ7sOZ/m3C5dyza8P8+539/CDVwYYrdSJmCZqepxywSL/xnOsW30MDz7xLMuPOZYN+Spfesc7WZOQaKaiWRNUqj51X6ALaCgoWYpcQ2HokphUNIQAofAkCDcQaTu+wAgHSKWICVJTgSSAoD8pfYFmKLZOwkEH1v3TStYet5ClR8/BNAPxY67YYOuOQZ55fAOVw6OcGIacJQhrirINSVPR1yqYLklWLJBk65Im3aVsWVheAzSBroOFQWOyxo7RPMcu6GOgf5xqWUNpFpqm4+AhFXiagdeo0ZRop6FFmczk8XyH4mQOq9zAcizcYhbQqfkuExWH5bMlh8c1ijET5baSaXhIx2HXYAiER1TTqXhB39VWPiFdw7N90rpP2XVpkgYZ12VFT5rVqyPsePVJjMECi2c3M2w5NBkhpqwGlnRxXUg0hVnYEqNRKHGgGkAIfE/i+i7S8xC+hiYgEY7QagQHkQaBjkMpH+UKNGHgODZGqoWGJSgc2k/3Ccsp7B8h1tyEpiscM0Rzu0FtOsNILU46plBTZfA1kiOTqFke8b7lKGGTzVapJrrAd3AbNqMlC8t22TORIZpI88RggTM6kvx2X5aSEUIpaI8IIoZARgxGsiB1l4jyaAuHKXo2rtCpeT55R6JJH03T8MMmI0dGWXniUu57dgPHr11A6qwV/Ndn3k1TUwIpNSp1j9e3DPLMk6/w28kiO361mXPWJjEIY86bzaIlvcHgY6pK7/I2ThsZ57bXbMYbLk1Gg4amKHuBjTWmPIgKupvjIE0QPlMVCEdNJgo26eiMRtgOJrtxQ0PowQZeqyo8aWJGFElNYBo+jgP5vE8qoag1wM1BLKSwqlB2gnOjkoHmUdMVUU3g6YGHNqZD0YOFaR+nJKhUBTVN4dQUJgqpCdJR8ExFvRLYak0NXEuBLQjrPn5DMu4Ljp44xHvOfj8Pv/ICz932PebPn8vRy1ew/qYbWX3hxfR0t7F9Ok9XQidnK5K6Yrpcw4v4bNm2DV+EmTsrTNecBQxu3kwjHqaa6qWjrYljUhP4UiCEQKIIKY/jOpPve8NV83yrdmju/HmqvSlBT+9s+ub20p5u47Nf+rr43za/mZS9vqSpnr3xX3h0MIvCpKO1mbGKxRtPP0ZYd+kvujhKENcUUkgyrkbVapBo7iSVTHDo8GE+eJzg1geGaA0rSi48+ZNzSZgphnZv5No/jrG8xWR/I8SlSxdw5gXnYKY7ODwwQGzWHB7ac5gHb/o+R8ck6RA03EBO4xtB6HxMKpQWaKdMXRAxwBIKpwHFmiCqB7uVp0CTQW6F5Sla4oKQprAtKDYE6aji9bxi0anL+ORHT2Hd6nnEIhEULr7rBt8OAVIzEXoYjChD04of3/ooxT/9nmgocAhEwgrlGXzkl18gm6lhqQTRli5iZoiXf/dfRIwkibTOzXdupKs5zhnL2nho6yBRI0S+5kMsgrRtGjWbuh9YE2OGz9J1F7D6uD4yNY+x/oNMT2WYODxNKTdMTBq0hss0GxFqRpSygOnxKrrhIZR4yzAfVjpyRkPWX6jTm9Coen5QCZsKHZ2wL6gbIZTj8sUL+3BzRSK6x3TdZLpYoVLXwYCDk2U6+tpZ1JOiO2Xg1xX7MzV27xpk3JYU6ja+8pHKQJeClohBUvPRNEnVDZruNcvFVh66ECg0NMMguugk7PIEv7jlQ5SrLqJyiP7f30uirYd0OIxxTC8118Fev51c0kWveCS7WmlZNJehchNi22vky0V0zUSLSkKaxG141CyF1tGOammlXKzjGiYTU1NotQaPvDnCaKHCWfNa0WMCM6n44xaXVEhDKIeEVIR9FUyeXYWtgn6yKyXZQoWWjgRfufoMLj57JS0tTXiOzTOv9LN73wj7DmdIJ6MsXtDDae9Yhh6Kc3C0yI4DFfa88CCfPrsPP2PhVRxybhlpWYBOYyrPM5srPFDUSRk+iYjA9wXhiEKXPp94/wp+8qtpkkmbxeIIMQNicUG54GHVPWwrYA6G8LHdoG9Y9QmiCzRB3VXoXlAYuBJMXVBvQM2a8T6j0GdOWFIG75FuEHwlhQggEzoMVQRruhTZiiKZhFwxgCxLoC2hMJqCa+bWFcIVGFKgJ9Mk9CKNhku1BLmywPEE/SWfXdLkL0/cz6Hnn6EZxWmf+xQ//vTnSYU1Pnv3syxqDxMXis9etBBb6ggNbnvyMK31AjumBSedsZytey2OXb2K119/kdZYCNdX9Iah4rmUPUlIeKxrjbDFTnHM6lXEY2HGJrJEIga5TIE/PPSI+J+wWh1m1O0CjpRscfb1t6orT1tLU+88nt60ETfcQqso83peQ84gu3zPoyrFzMS0wdrjT2J4+CBnnnoGO6bytHaUscsVTGyuvX0LZy3S6GoK8eZInU+f3MboxgliIZNX73uYD33/3yjssfjT/gH+cNMt9MUCu1BIV8GKr4uZ8CKFowdTs3BI4AlF0QXDC/p8s5KQrwuqDqRCioIblP09UYHSFNlCAKjwDMXDOfjmdy7nU5cfT7ncQPg2/YcyDI3mOHA4i+14+J4ilQzRNytNb1eK9rYU37juSlb8+g9c1OlT9yGidK786WdpCzu88fJ21lzzLaoHNuBYFo0lp5Pc/zTTufm0tRmcsqiHF7fvpUkPM1hRpISNV5XkrQZIheNraEJgqQjzZzdTKDVQWpz47JVUSi9gWSVAx3JqFEUYzYR6uUZzxKa1I8y+nEfUEPhaiJ6oojuq8UADlEIAACAASURBVI7eGj1NPpPTCX6+34Wqg2YoZDROT9JAhnRMQ7LAq3Jo7wQyJMg4DlalTNWWNMUErusypz3G3LiOZ9kMjjocKpTJZgpkKg4lVyCUQhMaIanQ9YCjaOth6paHp1wsK/DBayoYsoR0SdUX6IUjhMJthIRLsTgIYVBlxVhxFH3ePOa2JGhLpxjYdBjT8+iaHUU1RTmwcYK5jech3szs1iiNdBqzpYX9D2+l3hoNJtHlKTS3hNmWILFkGcdGFhAy4fNfjfP4+mEefOZlCrUMZs4gEoqhpIdruRQI4Qkf3ddQjgd4eL5Hya7zztOP4r++eynxqIGhm7yxZYCrr3+QzduGcBWsbJLUfcXP6j5CN7jwrKVcd/WZfO7Kszj93j8ScSxc4SNSEfbumCAsdApWmVDG5oLPncLnQxlu+M0eLEunIX3yNTh2Xpq7fj+ObVXxPIO6EYViCVwICUWpAoWKIh2FgiNIRhWO8tE0QaUOId1HCEHRho6UwKkFTqxUCLpagom0ZwsKNUXChKojSOhgJgSOrfD9YKgZCkmKmsKJ6kSkYrKq0KQiGRKEJDRsH78eLKroJhouXd0mw1MFIjFF3YV0XNAqYVfOp0OTxH2Hdae+h9efuIstjz7F1CtPk/ZsHt81RsqEBW0mU4UaD24Y4+SVrbR193JgYBt2TwKRMom3ruFdZyUYPLyTtcedzM4tz6K0GGEBQzWfqAFKkzw7UWOOUWI4NxfncIZoKMYbmwbY3T8g/npS/l/HZzXzQwo4UPLF02/uUXOPjOBFkoyUDtMXMYnpiqm6R2/CpN4I2GaeVaNv/lEMDw2Qy00xNT5MsqmJZctXsWvvfsJOlhe2jPPtz36YkSPjCA6yZ7hAWPe59pFN/PjCtex5fis7Q2le+uo36DQkUUeRd6FiKZIm1O0gW7mhAsJH3QZdKHxX4LqKqWoAsA0LQTqhCFuCUBgidqBb9LzgeBANK440BHsqivd9+GQuOXMplYpFPJHi53e9wJdvfAjX8dBRaICjAvlCKKTT05nmA+85hmu/uhhl6AzkLIo1SM5NsX7DINb+jfRXkjz2lRvoaU0SjegcrofxttXQmg+yd0KxY/8m1sxvZdpWdKYSmKZJPlsiZQpcpYOSFF0Fhs76F18lFonQCKWoZibAVwyOjOP6zgwaXtLnxxBKQ4/FyE41iGouw5MNaqrEm8AcU7J9KkahWifiVpgVhokajDck5y8JcXpbhKzlsH+yTiYMqZikXnDIliXxqKSryWRyqkZnd5y2lhYmanXcWoODk2VsyyNfaVByZ6xBQmK5ASTAcV0GM1VslUcD6vz3106fmex5M6+ol8IuHOH2x/rIDryIc6TMrv0TRIDZoz7N9RqmFIQrNguETzgtseQsVp7bx5/u2kVudApScbR4gmZnL+dfvobcyDhTQ3mGLB3ZOp+MJ8k+fQAlwZCKY5bP4oKLlpHwKqx+x0Juf/IgT932IK2xONlqBUXlrYejXdeY3RLhYK7KZZes5YarzyIa1jCMME+/eoDLP/Erlva0M2/ObHqbIuQqDQyh8Q41xrilcf8j20FI7rnvndTKNne8NIj0dHI7DpNqjzE+XUNTLn44QvR3j5J1NZYn02yuucRjPi0Jl3Fb58BQjtkdJsp1mUo1s1BziGo19o8oWlICva4wooK0GdjySpbEbih8CXsKUKoqDAn7c8EwRdOCibTlQEgH5SlcN7CuLogLisC+waBQUn7wN5E+WggOjTpUXbBnBhIzdmk6NFg1S+J4gkTMxRaC8SmfVDyM8OokYoKxaSgWBCEDYmGfmis4XQred+EneWPHQ/zsmptoj+s8su0wEqhXLfaPueSdBjd/6UQefXE3tiZJdi+mIxLj8KEBqrUCMSNKZ2srNQvSEZeMq1PxFR0RyXTVo8XU6EzoPLNpM8mwQa5YYmI6/9aR+X9OqP9fVY7ndEXUEaOJf18Y5c5xi7wd2OekEISkT93xsTSTlStXMzU9zvhklkq5Qrq5iaZ0C50d7eztP8TBwSP87qsn8ebWfl7b0M9ADq57Vw+PDxZpTS1gxYI5fOf2B5knYVYokNY4QmD64MhgQmzoAUQzFVVEQ8G/XXQUhXwwELFn5DKagkg4ODqXqoKmOERCgbZyNOeztwZbK4rf/ceHuOTdx2A7DtNWF8es/Airk5JECDRNMVkNxOFVFCUfJhvg6Tp//NVn+OW9Wzm4Zzfd8zpY0NfK5ecfR9uceUjlE42nuePXD7Nxyz5yhSrdHQm++rlzaenuItS0kJPWXcHxLQb7bYPmpEZhLENMN6i7DpaS5BpQajR47v7P0tQ5m3Aowt33PMX+gUHOPmMFi486ClekuOHmX7NvxyDSDuxkWdsh3hznqitP4txLziKfsbn5x/exZfMe1p0wn09+6FQ65/YhpEG1qnPlP3+d0xM2BTuE5QkWpgWlQoNpz6AjonA0n/05xayUSVcqRDgWZt9oiUypTmrFIp5/YjO98TCuHwyHGr4kJAUePgfzVe687SOccMoJmMJBENgA645EKYVSoBsaIc1HeQ5CQqnRzM03386i+Une995T8WQYX2hUMhbf/8/7yY5PkyiU+cjnLuQ/73mBU06cywlrVzBvQQdjI1niEYOJAjz11Db2P/c851x8FqGQweDufu554yAfvXwtV1x2GpaR5vY7N5AdHuCmq85iaLoY4Lek4gvffoiu7hhf/szZdPb1URNNfOrj36Z8+DDhnnZ+88PL6elqAiEZyWp8+V++S1rEic7p44zjutCRXPfzF+hMJOg6ZhXbXriPXF2jLkJ892vnc+n738PI+ARKGoSkzejwNFIKomGdVCKCFo3hmW18/au/ZWrfm7TGFammCJ2tKe57fIw5nSkaToUly9vRB4+g1ytYx57I1HCR+Yf3osIBXNZVilJZUG347GzoNK1cwEcvXcvJp6zB8SWaqWEK962HX/31pKiF+eZPXubwHXexBY31j36Frp7Z6ARWFN8XNJz/XjaMkMQQHgKF0ENs3gN//PgnSYTB1CRSUyRNhUeCjnSDRsPBc6HmCZJxSIYNDo361C2PgTHFvlCUL179fu64/QH0TIHlZ/axYdsk+Uqd6Qp87gOrueXebVx62fsZGx9ECsnBQ4fQpCQRjzKvby6GW2d7/yE6Qx4lCzRdwxcBgb8t7BEhxOuTFSZzBVF3vL8jOfw/Opolj43XxZmLm9WLJYe6EnjKR9cC3R5KUFA6slxmZHQcdEFuOktLRxsNV2HXK+zcleGsM07HbuT4zK07mBPL8Ynzutm1x+LefVVe2lsmylYee3Er60KSaAjydUXBDvSDuhNQsGe3ihmah6JYDxbBWkVh+YJUQjGZA10L+j5K+FgNAZoMpmFC4FUVuuFTcWG6AbqpE4uE3iqZ0+l2muMxwnYV0xcYpmB10idXF0gpyPqBvS9jufzwtsc557x3M/b0K9Qci/u2DXPo8DQP3PZhQPLgC0fYtjfLnJ4OSlaZh554k0VzWrj+6jOJRcp8+MrLefr++4l6DUpZDcMwcHzIKkiiYSiH5Ud30doUZk53knOvuJU391bITR5k4HCGFx9cw19emOb157aTjscQAuqNBo4n2X9wkj+u38pnP3kR57z7OvzpLLbUuftPm+g/MMFffv0ROrrncdJHvsvskM2WnImh2bxnQZyBsRrDNUUsqpFxahzOQUc0yrTTQCtDPVdlKFtly3iV335tNUcvO447f3AbqbiJlAZSKXzhUWy4yLDOrLYYf7n/MZ5+eQDP9QiFdH583YW0t8TQjQjrn93Nb+5/DeX7OK7HPb/+Mm/unuDuP73E4jnNnH3KIkSonX/68C18bJFHrMVgcMF8vvPLx/jtjy5jyfL5PPTIJs696FdMTBUJRUwuu3A13/vGx9l2xcV879rvMi8l2DRYJmUa3PCjJ5BCcc1HT+EHN36Ysy/6KgcODdPRnqBRdnn01T30NId54qWD7Bn4LZue/CY/uPFumqZG2F60+dnXT2L2rDS+7xNv7eGLV/2AL1w6l5fuH2Y8M83zzxR47I1d/FN3E5Zdp3TkAO2zV9LZOMSWoSo3/uQJ+rrjrFvdR7xlES3zP4ChyQB8rOkkExEuetdyvnrVWdx+xw1ccdI56FIwMVXl6KOSxGIJavX6TKKgw2E3wr7hCv/6oRbmvO8ybrrkGo7rkkgn8PxXHLCFIG547N2wj89uO8z638RZs3I+Dzy2hXsfevOtckgBruvR0RLhox/5Z979C2jpSdIeUvz0R/fQf2gK1/WZN7uFb3/5HHRNEIun+Omvn+LZV/vxPI+mRIjf3v5tbjGSrNLL1H0I+6CEoGpXKJYCyZBrBwzLqayg0HA5ps2nYktmNQvanTr333w7NsFJ76knDnPF5Yv57fpD9HbGue+VAu997wVMTQxhNWyyuRzNza1kMxnsmgUKtuwbRHgWGc0kqQfUKlP5VH1F1NNZ1Kbxp31V4TreWxvCP6xTVPggNXqaYxRcQUh5NJka+FCpNZiqWqhKiQsuvoymeATDV/R2tdHV3s4pC9pRusHKRX28/srzdDe385mrPsHcVWdzywuCsZLFS3vzGALSEpZGJW0RH8cKbEgLk4KkgllNsKRdUCgGOsC2qCIZU0HJHxd0tigaNWiJQ8IUJEPBQKU5HUyFIzGwfMWhPOydVFRn6MDKVxTL9beK5JB7hG39D2OccQr7HUVE97EcSUKHZExyVLdkURIWJKB/6xG60zVMA1q7mkjoiky2SrHcQDMjbNoxwjHLFzCrZzb1Sg4hBaZpIKRGpXiED73/nYxPZjE1DVNKdF2j4Pos7ZmFY0iGChU+dtlaujuSyGQXNSdK35w2wpFQ8OX1JBOZOolwCJQfwHVVKEgaQhDSJZM5wcCRKbxQlKob2LCKJYu65VBXSQYHhshMOpTrFkelJOv3ldg57VJo+OQqBQ7kFLOSJrNaQ+hGhKIrmCrUGS7adM5uZen8JDdc92FkczP4gnLDBuXhuFD1fM48eTHrn9vHNTc8zIsv7OWZF/fz5EsDDI+XkFJiRlv50R2beOa5/by+ZZqNu4ucffG1XHb+sSjgyFgRhMARcbzRYZr0KDsrdX7+5FZuueFC5s+dxZPPHeLH//Eo71kYZWVbjKQU/Pyu17nkI9/j6JYp7rj3F/xhwwgrF/VSqdsYmqB/MIvUJNXxTdx398388K5X+O7NT7F33zRLZvXSN6+LrrYIxbKFH2tl38EcuzJVVq2ew+lr5+P7wSP0zMYxWvKDHFl/kGrvQkQ4ilbMce3xnZy7tplvXDmL2NBmOnqPwvYUfakwkxMFfnXvJjRN4pmtzI3CpUuaOX12jNlhn3KuyC/u3sCTL+0mZo1yQKUJCQ8zYpKMtdPVE6c5qaFJKOQc8r7DlDBZuWQ25525Arl0HtGYwhZgq+BluApDgZRg2R7likUo1sZv/ryfXS/tZf+GvRzcvJc9G/ay5/V+Hn1gO5nJw/hC46OXHM+Hv3Av9//mJba+vI9XXunnsef3Ua4GiZy2bOe7P3uBPa/tZfvGfh59dCefveYmZFc7vqcCLFlD4LsBVEKP6IQNaGuBsCmY2ylY2a2QhmDJHFjUoYgnBe1JycIWSTxm0NsW4o2XhtFbOvjYZ/+Nd6xdzeTIEXTVoDls8IEz1zE5OckZx61EMyWFfJZLz3037W0teDWLUt2mVnMQQtEd0hA+HCp4HDevVfF3FsS/K97uSseVKwyGHQMVTlJGI5JIc+bpp3PhuRdzzec/x4ffcz5WoUguN4XjehyzcC6XffAK/GoBx7GYN2cur2/bzXvPOpl/WjqHFcvXUu86DYAIkoSAiq0YrktKStEaVbiuYm57UMKOVsHQFZoBZUdQrwX2pphS+LaiJRGIt5UW9EksB8pl8PUZZb0IrICJaEBqievguR5/fmInpUqw83p2AbOyjQd/9SnWb7qHBR//GJtkmExN0Wp6tEroSUFXQrIi4vCT7z1IeypE0oWY6TOdrzCZqWAYMSYmsthOmGoph6ZJdA3amuNB9e07LOiS9CxbRaFUx7M93HCa3hXLKTsOmVyZtav6OOMdC/G8IIxH1wTRUAQhjf/udgjQfA9DBBCAhrKCa4BC0wJ7ogDqjgsiwEOJmW1RCImuSdIRjaSCvSMu4yWPQt2hrjymLZ1ZrRpNUZea30DUa/iNKtNVlyMVh8vOX0lrUxRrYisf+vhlTBermLpGyNRwlIfveiw7ups7/ut51rRG6ImHiRsarudz4EgmkOe4BuMjk3SFwszu6WN+eztDkxrFYgbPV+w7MInnQ6ni0SQ98prgoV1ZrnzfCSxd1EGxEeX6m37D1UdrvLfLRY+nmdXXRiKs8+Lrh3jyxW2k9SFu/ek32L13kHm9aXRNw9A1srkqmUyBsLWXa2+8nnBnmJUru1l69CyWLFhHa/ci1EwQUjhkIpo6OO3EBbQ0xwBFJNbMs89s54OnStLL2xkdz3Jw9xHOn63R/o4mUnMjxFt0CqU4LcYUTRWf5lCgZKjULBABGDmpe0RUnTAwJ27QkdSxLJuBw1kkVdKpJL6I4fgWliaoFxtEuyKYusd0qcZIts75Zy6jpzuNlRvg5h9ez94M6FqgNRRCBAJuGVDp3w5wMXTJcUk4IQ6rIpKFJiRjJktbBN/63n28/73HInQNuW+QE5ok8+OSZAiqDYcjowUEMJaziFWKrGmSLEkIVqYFj/95M8uOO4qDmSC4zQhLzJRGMiVoSfl0NQf073RTMKA1YpL2jgiCENU6zG8XeJqPbfsk8DgwbdF53od437lnEremOOec89i3/wA9HV1EIyEefvoZ+re+wQubtpKKJ5menqQwMcydt9zCP192ORee9x6OXXUsDcLkZISKFmVC6cQjCTSp/X9ztDQc58ztU1V0X2C5Ls2JGGEDRocGOXxwJ489/ijfueVHnPbOd/Fvn/oMui54/tU3uOqr19M3q5OQabB91w4O9u/j3vsexAyHWbt8KcIIKjRT+ISkYFYMTOnT1xzEhXakBBNFKHvQEobpusSQQTVoRhTZGuwdU2SqkgNZn2IdDKXQdALsejgo06u14EtiRgVmWNGZEBhCEZPwzMv7+fwND7FrYHImWNylURik2Rjh61efxsYtv+e8n1zPru7jeDkTopxTdCqfsBLMyeZp9hy6Yi6GbpDP18nkyohQjJHDw0RTKSLRTmwnqN7aWuLU6i6DI3ns8gg33/g50n3zmbV4EXN7m4naJbxijqrtcfkFq+loSwSLohCgfDyn8TZAW9Cf0zSNiG4SMQ2SRgghxFt+TTHz4Ek/yLQWb0s7E0DNhV15h6zrM+6BLxSGaVB3fGanQJM+I1mLQ8MlikWb8ZLFeNmhoz3Bmacs5cp/+z3TE0N844uX4jc1BW0K1yOmCZLJEPffu4Fj0ybtmqRTBulvvuvRPziNUIqSIxkbmyacSJGgTufsHtasWspTz+4g3RRjYHAa14Ny1aV7UQ+bDhbJ+ToXnrWMWCTG068dYVGtzP4pj/XTcPza2YQtj/bmOPW6y6PP7qOSHeKs0xYzVHKQjkvdduloS/Kzu9/kGz95klopw9olBq2L1rB9d5YI7STao0iv+NYJQmkamUKRY5f1IGTwOy2WZrR/AH+qhTf3FRkr5Gmb286vd5fpm/Io7rD53TMhvvTDS9j00m6ceIpM2cefEVfPSKjJWBG25YNcZsMAZYOu6Sw7qgPXA6tWoU02EHWPoek6mVKVNj2OlUhSrAXMxW/+6zl88it/YtfO3aw7vouDqQ5KNRWAkCXU64GUJmT87eBAAq0GxKKCEIrEmlUcdewKIkpROjjFdLHOk796gnf0Cea1QLvpBy2rms3QaB4h4MB4mcUR6ArD7BC0hWFNE+zc0Y+KCdoTEDV8DN9H2JAK+YRCIByfiAmd3QZh6TOdrdEo2ygfXtvlcfGyMImQIl+HNqCvp5e+jnaqtqKpKcl3vvcDBg7ux1ce3b1z6VuwgN5ZveDD16/+d2JtXXzh+m+ycfubvLFjMxOZSSJREzSNquMyXbI5VCz/X+rEv7Mo5ku1Z4YmJuc7bp2DoxM89+Zuntu8h/WvbmZifIQlHQmOn51iatdzPPLsc3zh41fhuA2OWrQEyw+xfWCI+/78KH++/25mtbWQL9c468RVbN64AU0IdCEIIyhZYCvJUCHwEdcais4kJM3gnzuqOUji06I+YV2QiCm6OgSuDxEjEGY3XMgVoNLwKeVVYCULgRSKTFaRywsKJWhH0BkS6JbNvQ9v4Yqr7+FHv3qBwaEcCIkmFJX8EG5xD+8/tYUH//xlHnzj97z3P77NxvhcChWFJqAtLpnwBGnPpVq3KZQa+HqYbKGBkz3C2Mg4vhM8CE2pOBOFMF//weOMjY2y7rg+6uUCPXYWVSxQHhvhUFkxb04bF529jKlsjRffOBi4a5SH53kIqf33fRTguIGv2LI9PKkh/sdtlABSRwW+iL+FASuQmkbVkRhITMfD98FAI1vxGZ/0GSvBaKZO3nHJ1iU52+WrV72Tp14Z5emX+3nqpX6wjvDNG6/BbniENUFUk/QBy6QdLJBCEQ+bNHyJ6yvGJosgJZVKoGn0HZejettpn7WAlq5eBCniySTZQg3H8anUXTqyWTb1Z+ntaWbh3FaMpg42bdpL++wWTlvXzlBkIfnxDCuWzcFXgrAhOHg4Q7lqkzCLHLduLfsOTWLqEqnplBth7n14K3f+eTPVXD/fuukK7nhhgHioQnX3RpTQ3npgLMsCoejpakL5ambzNKjni0TaBfGqYu9UGcO3cZpb+NIOiTh9Db3L+7jjnp3MmdtCuWEHC+LbxpsKjaZYg/fN9dg81eClIzaTSufmr53HPx0/H0uYuIUcYxUo2T4JXVCpudQNjVq+zOhYibNPX8ZQxuCZl/fzwBO70asH+eGtN3G4FGhdI1oQ51u1A/6o+u8Pp+5LdkwL9k1rPDQt+MZ1lzE0OE17BBakBPbrO1keUVQqkM1AQhNEAKthMzKexwhFGDwwSmsUElFojgUxwoYm6DpwgE4pyFcF1YqiUoJCxWPjQY3N/YqCJTk8oRgatcm5Jom2KHkUDWDNHMGj2yx0XxAxfGaFYepAP+mmOLO6utix5U3Wrl7FiuNPwxYhHGVw4rqTGRo+wne+8T1u+ekNhErDHLewlSbDZ9e+AV7a+CZvbN/Ppp37yeWLNGybwSPjwvu/xKLKvweQLZQbh7YcGBMJ4dIpFHPDgdd3475hfrn+Nf7w9IvMb09w7Wm9DGx+mBuuPB9Dk1SF4DOf+BwLUpILz30nfjXPgp4WhoeOMD05TVxCs5RoukJogpYI9KbANBRlH4oNiRkGX0DZDWIDrGpgTldCoIUCL3IqLPjrepEIQcSQdHWAHoFwciZSQBO0pBVdrYK4FMwPwTFJwbyoYGpoiu/89GnO/9jtfP37j/PC6weo1hw0TcNzalSmB6Cyj3PWxNjy2s+4+HtfY8pVVM0wCalhKR+lFNl8Fc8TJFWeRt3HcWxsx8XQJKGwQcWOsv7ZPTz05C688kHOvvh8Nh2extc9xqs6k5kiH37vcfTNm8fXfvAUb+4YASkCJ4XvBOynmXviuh6aLnE18JSP5wTDCvG/+sJ/hQD/7a4YktATN5jfrDErJok3RQNsve9RrPpMVWyKNRdhRqm7Grlag0gizEc+dB6/vutplOvzwGM7mB45wKUXn0hBE3TqHj2Gz9yoJKEbxHVJBZ0tFYEzU+fmCnVcpTE6nCWCTlQDY8FxJGIaqXQL0WQcXUmqdQfbhdHRDI4RZrjsMH92K+GQDokmBveNcdmJHTy/00daWRxLsrU/w9GzU3g+DI/nqdYsvHqBd51xPL4m36qWhRBYtsd/3vkau/onqR95nd/ffj0XfPEXOBLscvWtssr1PCJhg5Cpz1xBBWiokEY5V+a44+L867Ioz+6ZJD+cxZos8PKLk9z34CbyhSKtqQhTFUGm5gYDlZn74dWm+MK3b6Dlso/zwL3X8frG7zO44Rt8/PK1tHY2869fuY2EcsnXIR6KM3BgFM0T5HJlEi0hcpbLNZ98Nx/+zI+JSFj//F6GR8Y454xFqEVLUZ5A6D6RuAiusTbz2ULQqOV44Hf/ysPDt/NA/y84sPVGjl58FE6jQtqUdDcJlvVqdHdDUwqOmq9oSQcYPaWgVGmgm1HGRyew6pApB4RvbJ84gqgJ6Wgg9J7VDZ0L4kRiBl0Jj2Q0uAjhsKTmhvAtGDncoFYNbLa7xgO4RHMqKGCmlOBPd93F8lPfRXbPBpp1QW+7yXevv5ZwOEHJhUW9HXzpYxfzxr038pNPXMzA8Ci/+OOz3PPURiZzFZQP85KCbl1heFVGxiaF73n/f4AQClB4vo/hNJjdLFjcIeiLK97ZJRGeYv9Ins/f8Sxfv/Nxrlk7m/rADi4/YR5nLTuaD1xyLqXpgwweGMAqTLPqxJO49NNfREoxs3H6+ApCAiKmz2TZx/ZmbHGuR7E8QwV2FQ07yHB2VYBJt+oQneEexgzoSAZEs5ipMITCrQsqJbCkBNenUQXfUsRC0GbAnDCsTMLRccHckCI7kuUXv3mJD119D1dc83t+ec8GBodzKAWaFLhujdrEVj7/sZNpO/c8/LCO8nxcZSCAfLGG0/CpuU1MjY9Rb1h4dgPT0IiGTVyl4XmK+9dvJ5cZ5fOfvoiiJ+g/UGZ0IstRCzq44MylZAtw30MbqdaCHFvXF1huA5QXTNI9m6VLFqADYQIVviaD6/j2Q5IK3oz4P/xeF9AELAxZpEIgXIXQNKoO5JwgrMiQEt338HxF0fa45LyV/OZPW/Asi2RTnMGRHJWaRVJM8LWbvgwNh4Su4WMy7Wq8XjN5o66hAYbSAMl0tozjSEYnCuiapGfxQuoHdiOkRPOrRJvTlKv1QLqBxuhYhvHpAraviEbMwDggdLpaBctaG2zICaxyg0K9X0bZ4wAAIABJREFUzpqlHTRqNqYUNCwP11N4jsWSxbNpNHwMwwAFnlLMiWkMjxX49q3PkC8USWpH+MHPvss9j7xCSLhvM3mJGQ3m3/bkZh29ks51H+eVHTnCkRCrukyyUjAwlWNg316UUkwXSxQms/TMSuDN6F7/+n6rPMh7Tm3mgtPnsnC2weTgYZ59eR+/uPs1Tjr/Rzz/yGuYpo5lu5x6TJpXNzaIJU0mi3WmC3VOWzufih3GrVkk00nKVYvpbAV7ehcf/dLH2V8JLCZeIwCnuG+rFA1NcPttd3P++d/gvPNv4pRLf8beff1ITSdhQk9UIS0fuy4wgEoj6OkbMlgoRiZKKBlmcjRDLAS+7yMQhMOCmOYj3AAeUfcUtgNhUcfAxQwDWnAh4wlBOgkyLEm1x4i1Jkh3p1kwJ8mKxVFSSYNTl+qcschgeQy+9u9f4ZgTTsI0ffbu2EbEHuRb136dDxy/mKUtPvPLA5zY18r7vncP2/sP0N0kOTqtc8NpYc7sC4Toa3uhXLP++I9yFv8hSs54Q4qaBZcuC9HblUJ2pzFCAl0okoZk/Y4RWq7+HcOZHOb0GN7uJzm4bQeTU4r0oV28o6uJ4UN7KDseQkFCQncIlrQGxI5SQzC7RZIKCZI6dKYlvW2QMn2iOnSlIN6mSLeBZvg0JRTKEJRqAfIrbyk0Q6CkYroosX1FozwjatUF8XiwEBfqCk0o2sLQl4Cjk9CNYklUsCAi0Ct1Xn5pP1/81l9476fu5Cs3P8qbO0dQCoTUqE3t4vb/+DIDJRjP1ql6HiEJtbpNreETEj6yViMznadSt4nHTGLREPV6A4Cde8d55KldtHTBqrXryFdKVFyPKy5axYL5c/ngp2/F/+tRy3PRpY4RMvHVTKWoXExdIx4OY6KI6AJdN9F1iXpbRSgJWgu6CEK1/marE5K66zBWkUyVLMquRdl2cTyPsCFJhA0s6WNoAsd1MCImn/7gOtat6uSh3/8bf/ntVVx2/kqe33CIWnGMqz7xbvqjTRxxDDbVPPbWNSqeJK4kCImSQcuhUGxge1AoVomEQhj5EmYsRte8btq6O0hGwkjlUa3aeFJjbCxDPRFDSN6a/IJC93y+86yFqmbIVxT5fBXp2uSLNol4CE2XSBm8pqemCYf1t66MLyQrWkP0tsZ55uV+7vrTm3iNHMcv0jjjsvdzZGzy7QEb+L7Cc9Xf1AnVqRHWfOKLvJCJc/9AgYG8GxBgmuIcmC5hKodjV68gS5hVXe0ULXsmTnWGwKLpbHzzAKVyhe0H4Nwrf8XFH7ud7/zwUZzpPN2tcequz8lL23hmR47WhItULvVKgwPDBT59xcm0xDweuPdL/PmOf+ELHzuF9c/vpV7N8YELV1OatZB4QhCOBbrbt2YKSqGF0jz16gS5XUNM7B2icHCCO//wPLO604wWwXIlqCByoGoLPBdKJYFyg+VkaroEukl+PENrDJoSAaS5VgPbCU4FjZnNZ7oI1bJHwxZMjYPuS1yhUy0JqhWPas3Gseo06g3qlRK2apAve7g2lIsehbxNa1Sy96UNnHTR5Sxyc8yRitce2EB94gD6gdc4StX5xXN7OPm7fyFbbGCYkMnXyNddHjikKEdTnLLQxEVQaHCp/w9yxP6hRTEigiCOE1fN5tPLJSv9LE9evYgT5zUzrzPC+xZIlIIbHtvFNb9/ntNXLmD7n/+TWUuXsq9/iBMvvYzvf/MHRHRBTCjSIvjkybIiFQ5iSS1bkYz4VGcYbdUylBuSeDRwPxQmBJW8wkZQbwhSEUXYhFoDvIagXlVvyVGTYUEs7GP5gkoV6lUwDEVXTDF11FEUhCKkK+KaYHFKMMeAtU2KlUnB0oRgtqnIjkxx2+9e4wOfu5s//GUrKIXvOiTCJcz5R1NoeOgiGOZAQOyJU8crjjE2lcGybQxdJx4Pk8lViIVMGpbLHfdvIrPnDT591aWMlOvMm93CJeesJFPReemVnRgzTX3sOoYucSwf/20NRSEUDcemNxocjTzPRicQ1Tuuj+8rdEDXNMK6+F832Fc+eU8wWPcpupJi3aXs+sRNnSg+Wd/mhDPW4Do2Jcfj0gtXk6uGueis6/jUpd/h3Iu+xw0/fpLv3/YcE1NFGpPbufKqK9kxmQ+GQAJiQhGTPjGhCEmJFIJcsYbnSbLFGvNbWynXG8w9+SRq0xncag0Nh3A0gRAgNINKzSI1pwfhQDZfwfOCazCQ99g83MBzPDL5ColElH3DE2iGR7Xh0paOEQ3rCMNk284BivkaIT3or7quz/L2MLOjGhFd8LO7XuGFDYewyoNc+6WLOPH0d6Lr8q1rXWvY5Es15F93Fh3McJrq1md4dtvzpHuX0zlrFvlyFTMUpuErWpvbOHJ4hKgRwisUSScSb1tUfaJNczj18v/ktt+9wrrj+vjpLTeiAws7E5hasPLGOjUKXp09I6AJgaugWHeY19vGgqOOZsVp1/HBC27iggu/yxeuf4D/uud1+gezWNO7+OGt17N3RBHxAqufpr2tPhKgaYLTumB5i2RJMzz78MuUHRsTRaXqYTuKmh04yqpVhVP3MWUgi8sV6yh0PDdoVVlWkMGeTIMeg5A5Q+B2JULXmSjpuLqBntQhIgmbLrZpEI3rJCMQCflENQ9delg1B+k7hKVNOuETNySGp0jbNe795Z2cfMIyXt0/yEknrmD//T+lvTXFaT96hK0jOT55cheLW30+OMfDTKb52OkdvHe+YpmWp6s5KBek/MdJifLvB0IHPytCisUdkt61cxFrTuKm332FaCyKqbnkCi6HSoqVc5pJRQ0O56qcc+sz1FWFF3/4Nc77zPtwSkOs33EIQ8EsKeiJgS4lLYagXBe0pwSaIRgqCDQVQC49DcyQwvaCIQoSPA9UJdDeVWxBS1PAlEu0asQToOsCw4BiPYBBeErha4CpsBBkLIEWMhhLzkU4AnyQoQAlpvuSqAer4nB8WrE0LemNCiYm8tzw4yfZvHMEXZfUskdYtHQRhVqDlCExpaBStynXHHoTOlJJQrU6ruOSiIeIxUL4vktzupPI/8Pae4brWZVp/7+17n4/dfeSXklCQhIgBIEAIghRARlEHNSxAIo6OpbRGbGi6DBWBAV1UBQVBUcpFpp0CJ2QhFRC6m7Z/el3Xev/4d469X1njuP/7o/Jc+zsPPu5r3Wt6zrP3+l6bN8zzENP7uTUY3vp7e3i0redwFErlnLll39FwQT17xsTrWhWG+g0K3ZxotAanNlzGcBkhRPTayiMGRW+EMzQQf7NUvdfOkUlqUWK6VgzGadEpk3eNdAoRsKAKz/7Vob2jVHwbaqx4kOXvp5PffFX+O1lRqwCynZxjOwKeu9juyGp8Km/fTPlWb1IaeJLaJMqu4oKSSxMNBmgVGlNmsQYUQszqFE7sJOOWbMo5wTlQh7pGCSJIkEyNTGJY5dQGvYeGGdyugH1CitWLKPabNGKDJJY0YoDqtMtDC2J4pRF8zoo5B0ELn+470ne+e5LKLa3ZYcamllFi2O7HRbM72R8osG3bnqYyekWzeEX+e4/XcYFZx/9F3G/TjU7dh/Jru4AUR3TFRweOsTgC5v4yW9+zWeu+RbCdKk36nTnPJ7ce5iVi/qZ1VPksOmxvLuN+N83KE47YZhy3c1P8Itf3sXbzlvEtd/5AkemmpgGCAeOmWPz8A5Y1qYItEEjipiotvjy32/kE1+8naUFj86yTa7s4VuSsfEad92/nTSscOLKMkfmLMWws1Q9rf7rVCxogB9qjFCyNGkw+8A+TCnQWpDG0KhmWsN8h8ArZSwCKSCMEkSqqVWqREkGjjAtRRRoDK3I+2BYGXvRRNHpRLgqIqdibJUV3kKS2UWFFlSbNjnfpN036M4r8l7GKdUzAVkLZgnm9Uh++P0fwIoFbDx2KXf8y9UMTje46EdPkipNf5vg6X0tOh3Jp59ICRsRZ5zcy7lnn8D3vvsOaqrAWUebKKX/Ml8X4j9E7v3viqJA4JmS7rycXNkh9adOzunpSLH2w8/xyW/ez1lvuoGfPfgKsWmwelbCuuU5FvfYLOuUnLbIorcguPL25xkbO8CRBx7hKzfeBkCvENiG4FBdYMaK8Rnr4PAk1GJFyYVIaeIoSwZz7Wxd2tENjg/SExhFjTI1bbbG86GrJFCNFCEyqo5lQgJoqYkVOCLLohipwngATz6+g9f+1Rk8d0jj2BojzfSAhZxmTgF8G2aVBMfkFIs86LIEw6NVHntmH4Yh0UmAbYJlWizyE5qJoNmMMKSgpS3CWOFakiRNyLkWbQWfnbv2M3dWPwXPo9FI+NHtz1Ib2cWmuz/De9+6nj37Gvz4lw+AYf2Ha7BOUoYPHUCiqdVDGo0QIQRx0KDlt/FMUqTNhnYjzTo0zyaOEzygzZLYUs7MFf/ty5aKTiMbS7gSHCNz/oSpJvYcTj5pPfu27Ga0FnLqhsW0AoMtO/dRTyQFw6ZouximSbMVcf+je4iTBNXaz0f//jImq80MzYZiCpsQiaVTTKmzn11aHD44gqEFc5fPYc66k5DBJBpJmkZYQDOMSZIEKSQL+9so522GjlS495HdBJNDbNx4MjunW6g0IU4TFIpGNWKyEZJqOP3ExdiWxaGJmMmdr+JbYzSnxzONYKrp7HNYt66f9Us7mdNd4tGn93Htjx4nTUJyeh/f+PxbMFwnm8GheGbLwRl5FUTNKWYvXMSTd93JK5ue4tU7v8kCa4xdA0PkOnuRwiBMIvaP1Ng1WiPBpBYl/wVvbwCTEzW+8O0HeOD3f+CKd5/ABe/4aw6N1+nuzNOsg206vOWjn6N79iwajSa9PXnOPus0dry4F5nPUzd9HCEwDAtSze/+tJ3RiQZp6yA33PBlXprI5omJ+s8CFE0tBFNq2g2NKyU5RxAmGomm2YJym8DLQRBAmGiCP0NqoxQhLbRKsYws8rRSlTMRJQItNJbUSKDgptgFiZOXuGWHJDWIlSbWglzJplFXREHAyHhMs5EyMiUYm4apiqARQRxqugsmnbZgenCA4eFJhu+4lQOD43zriV28/TV9nHFUNxef4POeU/MciQy+d2GB1y5Muf7WvXzn9m0c+7bf8svnGty6RfCafqFXdgu9tN3QPTlLa/F/7RT/69/OcbQ+ul3p5f1G26KS5qHdIUvm5FhjTPDBDSXm5COe3dvgwJ4phFNiUX+RsDHGX53gE2nBG1YX6MgZfPTuLVz4zZ/znV/cQ97ITpwOR7OyDVJDEwXQjASmo5GpxDazKADDELQSSbMFJIIjIxBWwQ40jUpW6icTOLBfM1WDSEkaoWC6BfsnoNnKrth5R5N3M2J20dW0yMjCJ6+dhX/GmUw3NLUgu65HM7QdNZOKpiOYU4Cyk32kersKaKWx/QJDh4dwhWA49TBmQryiKCZpRdRrIfVmgEpSfN/CNG1SJE44TDHn4/kem54/yH0Pv0hfOaCrq49rf3gvy0oWYRID4t9maCIbRsVxQhin1OtNyqUCMZkEJrVMHm7ZDKYmqdKcdNw89hwcYb4NHSLEEel/mGcBGNKgzbbISwPLMLAND4OUEZXy25veyy9//ThtjmBPpLj+6r/hK9f+nsXtnfgywTNUJmIGLCF45qWDbNs9TFAZ4NK3biA0FFKktLTMDtaZZERLiAxpD1i2SaJjlpz7TqiOIqw8hmVRKJcyH60QSNOkHjQp7d6GaSrMOOJffvk0hw4PccZrFtDmFZmp5cQRtCLFWCVgwbx2Nr52GV65g+tvvJtjeoo0Bw/PnPwCjcbq9OnsaOP4uZKjFxQpO3DLb57jT0/sRYqEMAgykbOGnKl57Jl97Ds0kW2uW1VOOmkF28YlG859PX1zO3jqF9cxfuBP/PGen7P34DBtfo6x6QplERM2Q8Iko9T8u/YfV4DvGBw4OMGnv3Yv2555mGu/+nb+6l3vRTcqpLlV3HbP/Zz/lot4+IUtjIchn/7wRj7xpV+TMy1MIbCNHIbh45sS05Ts3T/O05sPQdJg3ZoOwvlrKJga9z/pFLUAX4KLQSGXaXy1hlJOgxL4XrZfatUyaocWkijJuqcwiokSRVivg9YkqSZX1Ni2xnYEjgHCAluDsA1ELJBS4lhQLJg4eZsIxXQlRbhgWRLPzBRL1Vb2b0idjdh8Dw4Np3gSXtsBn73wI7ztBw/y06f2U8gJfCfFcOC2pxrc+MQ0r05oHjjssGXK4uFdTR46lOAbCW1uxKsDIaZdRk0qclFKvxEzS/93dTGbwZt9efTzH+vD9fMMdPfR7xahMsm2A3WW9OUYnaritrWhAs3KtYugo4+BbbuYfeLJ1JsVqnv2sf3gIMNiHicc30YrN8K2QwlHJrbiFT02D0xiScEcCW0zmqfDtSyxrduDnKEpuCAtQcFX6A5JbVJR9CH1wfQk9rQijMF0NWkTkmYmrykv1dRrkrCZ+bIdIG9nwfWJhDAArQRdBUWzLpGmotuHb97wG+66/TrOPq+JfnoTx/RlCPVWoknr0N4HqRRMT2n2VuHjl5/GxjOWkyhA5HjxsWfo8RxipTAFRElKM0yp1AKkIWjGikjDicfOR0iB4/okYUKbazFpaMZrEbf97iXOPm0ZDWXzx7sfp8PPYwQ1lNb4rpXJcAwTmSqkEBwZq/LcS/u45D2nIw2TVw4OUC7mIYoZrrc4ce1c3veuM/nQ5//Aq3gMhh5RGhAosEwDyzIROmXZmhWEUY2iznpSrWHhnAI/uuQE1q1bz7s++ve82tJ84J0nkivP5fFHXmJBycexoKEEQkjyhiQ2JWMTde59ZDerl/fj6GFu+P5X+OwHPku5UKCoEnxHcrhuIIRBksYEkeLgvkHSIMTQTaRTRDXG0FGKDEMs2yBVCiVMGpFi9fJZODsrGEHA5u2DXPm1e/nWF0o8/uTPWb3qPDqNFJRiYKKB11Xg2i+8maWL+vjp7Zu462cP8u33FPnNMxFRFNFe8th/JEV2eVhlyVFtazhh/BVGBkZ44UCda254kGWLu5nbX85uubam3bfYMzjJr+7ezOf+7vXoJOaUExbygR0DXLTxUoxmhXdc8UZe+sYnWfa2y/jnKz/A1T/6DWE0jl9eiBYtUp3Rq6WUf+lDUg29nk291eKFrYf55Fd+z/eutvn6VRdz+Ycn+MxXrqezo51X9r1KnCo2nr6Uc994Gpd//HLOWNALWjEaJEhcPCfFjmKqQcQfHtzBheesojG4hU9f9WHe/4ZL0W5Gscn5FmhBK1AYEkJSvCiLQQ1a2VJFoUm1wFEaz4PpOgSSf5tL6gwMEccJo4FAJ1nAlWNnBKKK1FhuNv4yhxVWnyYSipqAak0wNa0wLPCNlLwvqDSzdyRnQqefUZaaGqZSQSUwMKIZ+YwHVnOKlcJkCshNJvy2WmFBXzs/eN8SDg81ueFPE/z0XYvI+0WShb3Y00NZZe8poPt6UU9tRh5/Mmpggn0HKszptjH6j9ZDSQNn+35SG6790S6+8WQozOE6YtaXh7NuiFe0OaMPHG1l11B7RsqhgHn5JzFmYiFX9N/Fq0PZDOShmy5g8oFnsHSRnUNNLn3rOpYt7OeWP21luhYg0UwpsLWgZAn6DOi0s+FnIqASCkppSi0R1KcVpSLIRGezjEBjWxAqSC2J6yqyhkrTmsgcGUIY2DrFcbNfTqgyplGcSHwnIxn7rsaIoNGAPfsnGd7zJPf9/kvc8KM/cedNt9M8OIgVh7SX4JUJk8jyKC2czZ1Xn8pxq2bhOSaOX+Q9H/8BfaQobTHdyCx2cZwghMA0JFORZiKJOef0ZbzlDaupVKa49K9P54Hf38/EwCi21NhS8tBTr7DnwBTf/sn95A1JhIGWBuvX9HP2a5fTmBzkY1eczxcmJ3hpyx6iZsQ1NzxEueiz45lrufX2R3lh2x5IQk5c289Jx87lsU17uPvuTfTkc4RpRKMVsWpFH1/+xEZ83yFpHOSeX34QdPzvxOCKuFEhjCLqkctEpcJVHzubS992ArPmdvO+D13Mb26+Ay+V1HWKo1O0aSATA6VSgiAmSTRJZYhL3nw8//KT1TRe2Mr8kkNLKVKdYgg1g6aTaKW44Aufw7Vjpg/sI7RcyjoktnOYgEoVoEhqDcbbFvDm/p38NDYpiITf3buNoSMVrvr4ADtfvJlvfe83DAwMcv68Ehe9cRW2ZfCNHzzI5675A7edOpvxZR477z7EsqPnc9Ypi3jP5Wdw+eWf5sxjJpifNOgb3MKyksdhP2LztgGuvekxrvn0G8iZkHdMlrZb7JuIuPWuzWw8fRnHrppFPLqbf/6nT/Djq69mKGmwdXCAZx8+yBsqt3LRBy7kgUd7eW73ISYnx5FOgfHpadatncsH3v4aogS8aJyr/+njfOdbN7PhpGM4NDLG/Y/tYdPz++nveZEffvcDfPRTHyM1OhkZOcIV7ziRT1x+Kh2lHP/81b/jp9+9lS47YU4hx/7pCBMX26xDRNbFxSlJGvG6DUfz+ssu5q47/8jn3n0Kixd0E7YmuPrLV/DRd+xjaWuCepbsQbmQRQEnSmRmiBnRt+NBztOE49mMOghTkiRBxCHlnEB7gjTUhLEgNTRCSVRLYWiJU4bYEqTKxuu0mG4FtLULjFShhESh6O81EEFKEEPOFYQSem0YnADf00xLTSuGcxcJ/vgy9M0RdI/BjlXz+cYSgzt3wovDCUcvW4797BbOu34X8VidSgwLCyb7BkMWtcFLU1ktq7ODBlCcQdqtKr7EUA0G/+JwEP8ZHZZtbg1DsKxN6g+uFYzGJndtC6jFgvmlP4MXFBuO7uDgRIudRyJ2jiS4xTx5U3NgpEWhUOCcDXNwmnVOP28db7jiN/ha4wnoMaHPyPRSRVtTT6AkobMAYZqhiQpFTdAS6BhMQ5H3BI0G+CWNrWGiKjD/XFBTUEIgpEnQivHsbI7SigWVQOOQOV6KeU2qJK+GihemQOV9rvvSBZx/1gpMu4jXu4ihfaPsGxwhDlvM7vFZOKeAUAH1WoBhSFzX4af/+gK/+NzP6et1GFMuQ/WUI9UG55x/HO9+1zv51NuvZEhoTj55STZjjBK01vR3e5xw/Bq27jjEz295iFRANUj49lUX8rWv3cOCzhyvVpqcfNJiXMegUg8BmNdXYM3qFfzxoW384Z7nSRSYtmT54m7WrOjl6KP6qTcjtu0c4bkthxk6UmNuuQBpynTQYv1JS1i5pIdmEM8wM/X/0eDUXvY5ODBFZ3uOOE4zoa0lOPaYJXT29PCe93+fsoowTZMwhtEk5MQTFzCrq4TtGmgtKOZNznrteu59bCebfvoAi0uCA6HBwUbEeKK46h/eyD0PHuKNb/krgkoNvCJt9WGaqcmrB1/hyS172Lp9L7uf+jqX/d0tbEhGOW1pB9c8tY8DdYWVJkyEkulQsWJxJ6/bsITFC7rZvmeUbbuGeHn7MAWdsnB2jr+ZW+DGiYC1yxfTXvaoVAM6211OO/l4Nm3ez77N23n38Z289Owuvv3sBEkKTcPm/LNWsvG1K7j7jmeZPTXELTvrjDZTzjrtKH789bdRLDg47WtYeOx7Ob4zoL9T8sJ+n3ceW0a7Fmdf8iZ+fPdL3PCzh/BdgzNPW8Ls3hLVWojS2QG/ZuVCOrq6+dqNjzA0MEJXh8nyxT14rolrC45esZDu9jIPb3qJMFAIKTANWL6kn5Url/Hxf7yF/jRhXzUijHzGm6Mcs6afBbM78D2LVGt8V7Ju7QoODQxzcHDyz8tv5s0use74tVxz3d3M2v8yfgS2m12Tg0BlGlgvKwfVRKIMxcuTcKimiYo5rv/q27jpCz/jGDejZwmhkIbE8wVJrGk1wTU05RIUioKgqehrg9HJLCZEpdmzaxgg4mwxKtE0miblEuwZSJhVBJUI9k0qLFOSVxCnmpcn4LPvnY3q7WTbuGLevF5+fPuzbN47xTnr5/DQiwOsnG1y/LIyRRRbD03RqTX3HcgMIHGkOXG2ycXH+/zy2Qp/OCRFJVD/DqL2f+UpZn+8oM3Qv35nNz97ZowjlYRHDmtcL8+SvjyD+49Q15r3n7uEl4bGeP6laTodQWgJtg4pNp5/MW88/wpuv+VqnnzkQXqEoCihXUB/XtOdE9RDGI+g24OCCzkrix7N+YJYZaJRIWGqnqHZEdBski06dIYxcyxoRCaTFUWSKko56CiA5QhaoebAMMyba5C2UgYDwbMTmsEQunqK/NU5K7lw42oWz+uks6OAYcrMXqwUqVIEYUwYJmzZPcx3fvQYyQs7WZSXhKbFkJlnuBqxZ7zOxrNXcuZr13P1Z36MMiT1OEEKAyFAkmVFeFLjWTa2I6mGCYHK6Mxz2/KErYCW0lSbEa0wc9QoLZA6xRUC0zSIUkUiIFEKJQTpzCbaAGxDk/NcXNtGaE2sNI04IoxT4jSL2VKQfU8hs6wUIZEzAn1jJuTdEhqtsutVpCSJMLCEpGBKOlxJ0RKYWtNQmvFQU09jokQTa4knEvosQZcFJc8nNgy01qSpphaFSNOgFLaYf9a5LFmzilCa5OMGVWFhW5LhA7t4+MXdvPDCDu68+f1869oHuPioPg5MD+M6RZ595VVGKmmWNxMJKs1mtqnXkLMtPEtScAwQmvacw4mzbSZyJrsPVAkSiW2AFyasWlli6cJuFneUSWtNXnpsF3cOhOQsQVNJervKrFpS5OzX9PLwb3Zz68sT7KslJMB733oCV/39ORQLZaL8sRy1dCMb1+e47+kmH1llUOwrYeXbmLd6HeNhxGMHXIb372Dz5m2UeuewcHY/f7j3Po47bj0f/8SH+PCl7+P8Sy5maHiCMIxwXJvenl6aYYxrWxw5MkLYCnj80UfI2x5FM6Wcz1G2QSOxbZe9E00ipWgG04SxQguLnk4bc6JOGEMkoa6yz0mfA91OptH1LYmZYRUp+wqlBFPNDKpStCAxBbFjYJopT+/POKOpgqLW9PvZiMo1s02xZUNewmQjkz2zix2JAAAgAElEQVR4jsb2BSVPECQSC4VtgTA0R4ZnXu9rHDRaSKqxJG8JgkamsIgSTS6FZ8eh1wHHFoyNKlZdfzPFyOaHt36d7S+8xCkLHPJGzHRdcWBa8om/28Ajv32SnVNwzlEFBqcVkhZTtZh2U/MP5/Zy3dMR//rClAiTLNeJ/0a7+D9KvB1b0udpXXDgjQsdzjtzMQNDR5i7ehGXfGU7Z60ps3nHGIkhqbnzmLtgKf/wj5/h4IEJhgf3seGsc3nTaUuZq0L8DKBBogV9vsIWgi5P04wkpgDD1Dhulr9SMmG6BokQtLdrGgnENU1jRorjl2bS/FKo1QRTlWx7ZlkC38jM8JYDUQt8VyAdqCWa7SNwOILxUFNLoa0jz5IFXaw6qo/5c9oxTcn4VIPJSpOBoWkO7x8lHJvgaAssJFLD4g7FoNPO5tGYvlkOOwcaTDYiZJCigLJrIiwruw4KMIREaoVj2cRxhNKZdMaWWchQMsOr1EjiOMqGvUAiLaQCyLaxGoGWWYavIJtTmWikNJFmJnsxycSzYZqilSZRGiUyUS4iK4ypSjFmNjASjS2yw8cVgpyMqSUWU1oi0NlQXhq0G5q8kRKr7OcM05Q6Bs1UkpcJ3bbA1hpHCmzbJKdjbGlSTzUR4FkOh9wyp7/jbdi2RaItVJpgmiZSJVTHR3jm+ecZr7cYGxjl+9d8kDtu/i0qqqB9l8bUNIfGG8SWD/VmphM1s4VOzjWybJogRNou554+l2CswvIVNtWKi+FIUlpUXhpl0fo5lAyBF2ryQcADu0Z5YjAg55qsm9dGpSaRUUiiIgYnmhxIBNunY6ZbCaZl8O6LTuDKD51BX98cthzM8Za3fghPC/oKsKHfpKenE2VZ2O0dmMLnuSHNkhNPIZ0e5R+++h2u/Mxn+PBH/pYzX3MCb3/nebzpoisYnxhHK41j2QyPHMbNd5NEmfd68cL5bDzlFPoKPkEk8WyDkqsIgwglYKIliJVDqipYRpYdtGBhkXlWQmXPILVE4soslzxEUbayEKq8Dbl81lyESZadYujseUpFtpQSrkmYpISJZmxa0JnLCPeOkS1wYikwhMJ1BPHM4ssxoZDLTmHT1GjLoKNTs2+rxsllJopWqHCUoJAXjFeyOpDo7PWdeclYXSMsODyYxZJMBgK1/gw++vnvsGPr8yxfvoZdh17mV7+9iz2P/Y5SDqYqMSef2MumzeMs7xa8ebXPK0egFQZsHYppRSmV1ODAWDLzv/sfI07/z19pKvjkcXzx5GPa2Tph0QpS1q9fzZ6tuzAKHdTqAVsmC5Dv5n2Xf4r7HryDS95yCVu37SSMI+Z15xidmmZi+3ZsKejLSXyhsRHkPGi3BVGiWdSfbfySmTS+aiNrsQuupt4URKEmVgLfyYSiQgkQAplAGGW/XFMKkliSoImTLLY0aEnyriaNNFEosFIo25k9sMsTxPWIyug0L249zKNP7mHTE7vZ9sJ+JnYOYEyMs8RqMV/KDMeVZtCBRIMZtcjP6uLEVT1ceFKJJyuz+e7Vl7Py+OMYGRsnqtWIgkxYjZjpyeIYpECqFC01KtFIx0bFcdahzTARUQKFyYltki5TMZEKXMOYiUowsC2BIySObWJYNpKMraiT7C4kZKafdI1s++f7HpYFKIVQmnKhjGlYSAEF28QxJCYGjgGxdGkoA8MwyQvwTUlOKrrtlFasSaXEMQU5L0esFJbp0e3oGWSVpGAKTCmQhp3JdQxJBYediUOBOnNWrsEwLBwdY7sOdm2cuHkEYfk4+SJdvuSo4xbTqAqOOe4otjz1JNVaitdWJm00sQolWlGAoTSGNDBM8MzMxpcgEIamv38Wy+f10qlqdLiKvr4CXhSiD7Rom+2TUymdOYeRoSrTdcUfBiPW9fpIbTLeanI4NjjSCKlgUUtTbNOilWiSVLH55UF27RtjxaISa1d0csFFb+drN95OzjCRcUgwVWd4YpojYw12bx7ggk99Bg7t5ENXX8cdt/+UVUcvYdeufTz/8MN84ctfwXIKdHZ0Mau/l1n93Tzw2GY2nLyeQrHIrNmzKbfnGZtqcnjz03QUTCqhjUBh+Q5plBBqBzAyS50w8GxJvZFj/jE+7bFG1JtYAnK2oIhAzBDqpxoSQ2viWFAJBHGqmapnhU2hiRKBIUDbBrqpKDgCQ4HjaqSAZpJFHkgBQSSz2AIFpZzAEmB7GteU1KY0dgqGM3O7SwQlRyBk9rzaIsutLnpg25JIWLQXJUXbwBSayTpMVDXvu/kWXt22h0RKXtq5k/XrVvOTH93IW97+EfYNDRIrk785I8dwRfGrr57OnlfHWDnX4Janqvzufe088EKDDlezr6Kv+p9qnvk/67sVL0/aDA5MccZSKHb28pPnatz06wbnXnAaXQv6OLl3mMVLjud3d9+EqRXdfW04vsus3g5u//0T/M1lH+KiO25nXiwYn1Z0SihYmj4Fg1WJZwmmK9ldUBgZHLOjIGgEmnogMRxNzpIEcTb81UGmVRQGmK7AChVVJEFLYwpFwQNTQ6EskAqmqmBJQTMCLaFbKno9gVYwbWZpgdrI0GVojakEHXmBnPFw9rRrpmOYmAZDpEw3ssTAjed009Vd4rEnXuZXd7/InqcfxJ8XcMmHP0hXRycv79jFE3c/yoGtzxC3QlxLIkw7I9gkkBoJuhVm/LpU4ZkmnmMRJymhSmm3Qsr5lNHUp4LAjFISoZCpQEuJ1gIbQb1RJyh3s2jtqex77mFKbWVy5Tx+Rw8iamFYKVONEBGmeDkPW9Vx812M7z9AoDxoNcnlSzSDKvVWgpQBOZHNIG0B3ZamlUoahqDDsDEE1OI0K0oixdcCYRiYGjzqSHziNEWj2N5yEEJiEbL8jA3kSh3UYwGFAk6rRiXXjuG2EwxvJWcalPq7qIcpk+ERlratoXvRUvbsfIX68CiOl6NaqVLuamfiwCiJSPFdnzAOcV0TEWtaYcJIFDM7sZidN4lUwsiWIXKeRWpJWhNV8vM62L9/H9XEZag+xRxXcrBl4oURR6om02GDJNUIlWLakvacpJU6TLcigjjlnod2sPfAGB98537effGpDB26j+OOv4RFF76TT175ab73netZcdRCjjv5NBrDr7Lhguv4yAevoCtfIrQsrv3U+znjzFOoxYJjVixg+5ZXue/uR3nj+a9jdDLipW27WLZ0Ec16naAp+dgn3s+t13+LVTkfnYI0DQ6PJbimQ6wgjpoY+XZEUsEwcph+maRRYbfRS90xWJ5MIUjAE5hpFjTmWRqtJVpC3oKpIBvDKCkwtaTkaWxfMplIgliT8zJnWRRk0qqcqxE2WIZEaI3pCmyX7OBUAhFJmqkmTWBxG4gJCGNN2deYJhAJJupZNkycahp1Qaw1uXIGWRk4lFBLBFNK87LXRjnfw/bWbsrtbaxesYQnnnqezrLPwQMvs2bta9BasDUuk+SeYdU77uPTF6/lhEVN9owMU/ZSdoWSufzvbH7G/+YlJ3QmX5zQFgerNk+PdjFVs3nzm87g4IF9jE6OUa9NU29OY5kucxcsZ82qpRSkT6UZIHXIKScdz9bde4kG9lKQkvleFioVmRKpNWOR5mCQOUqCKLMMjlQzDaNlQjPUSBN8Q5DUFYabRS8mKRgKtBZIrXF9iSUztqBlalotSZjMcBUNnQElRCbWjtNMnJq3IQf0ONBhQtHIYJgFX6MNcAxIDMHBMUGbp2mEgmIO5hQFcQLRdIPpuedSbpc0A7C8AkauTKNWpWApeo5ewNLjTqRj4WL8vl5yPd04OYNcR4F8WxteqYRtWyxwBZ5Q2KQUbIfFukaPmyASQVWY1BIwTUliWBi2h21mww+vt4u157+HDWe/CaWaHLv+BNadcSZ9S9dSKLVTypkUOmYzt3ch8xfMo69vHitOfB3ViYM0axHlng565nQjDROExDQEruXQClr4riQnE2JMGioLD8cwaZoGnpCUDMEcEePJmIJIaBcJvuOh0whDKF5JHBIrh5U0caRi8VlvBs/Hd1xEFNAMGtjSRAjwLJvpZg2R72J0325cx+SZrS9z+aXv4JE7HyB1c6hGDdMwMByHsN7MfOICHM9lbKqF61ocGauxZtl8Wo5Hr6igkgiTiEKYoANB1OmRDu/luZE8Uifc80pEoE0qQYveQoGpVkgjjEmFxLYEqU6RWmFIA2mY2CZoBEfG6jz89Ks8+fwrdFgTfO/aT7HpuZd577s/xBc+/1lmzenl7ttu4/s3foVKkmft8cfw+DPPoKdG+NMjj/Phz32R+//0FJ/98GdJwlF6l67i+u9cz6XvuZjJaoRKIkoFhyBK6epqY7zWZHDnfhIUpkyw0sx37JmaqjLotX2MnIOpBS0sls8T7Ng9zWWXz6N9Thcju4YIArI5vc5uOx35TBESpjBYywhUqc7UYFGiaaQSLQQqTFFKzgS6ZeQloeUM3EVheYJiSeNYgkJeIq1MwD27DDIF24A40mjHYGJCYxsCJcC2oBVkm2dhgu9DGitqVU1Hl0PNhFPnwNGfv5mpQ8NI0yJWBm9581nccsuPmTN/FZWpUaYmR6lVJ5gYP4xnC970hgu47tcP87vtCplUuGhlGy+NKlzLYM9YfNX/g6KoqeF98WDDYeOZ57Fu9WpWLl7MpuefxbRtKlPTNMI6Akmx0I5SEQ8/8BT/+PeXYsZNDo+32LJzL299y/n89ic/pt2AiQDKJnQ6gpIDZdvgqGJG1/YNTVungCR7s2oBBLGAGOoNjWEKohZIlZFvolDgF6AeSBq1DEIbx1limCGgkNOYliAMJVGqqccSlKbsSnp6s9dJDVIKIg1KQikHQQjEoJJso21LTbUpsG2BSrMRHTphuLCAv/7SVxk7fAjbdZBSEk1PkdguUbPK4OFRWs0qliVwCm1EI/tJohALBTrFECmeMJD1OoYEhUkr1fTmI0arUKkolOdR0xI7jZESlOvSvvx45i46mhWnnk9XW54obOE5Hl2z5jAxeoREO5iyRaHYhS09fLeE4xZp7+0jaTawKdB7zLHk9RSu20NYq1Dq76HY3UEURXT5FtgWaTOlaCqUMHANAWmMCywQLWY5CkOn2FJgY+FaEKkUxxJYhs3OlkanCVor2pespq2/Bz/XThQFYBhYQY3JBDwDJiYHcfwC4egITi5HdXiUFce+hh179nDJeWdwzz0PYxbLRHGI7diEQZjNyLUmVJrPfuHd3PO7F7HzBq7js2h+D2BTSitEYY7nxyNu3TlNsdTHzzfVyfkJd+9s0SK7IBxqCpa2QYpHI0lRSUKkM2G/Y2fe/MQwKDkCyxLY0sAUir0HJrjj/h1senITl719PRecfyZf/KcfMDU6zOy5Hjff8BvOf+PxbDjheHrNOtdccz1nv+FN5DvnYpsx3/nBPzN/4RosNcHco1ax/YVHeeVQhRUr5qKihCCMieOItWuX8rWvf5t5XR1UmppiLqEVZ/G/06Gip1wmiau4tofp5envarL71Rann7uAe7/8BNIQ5JwszQ+R0aiaKYxPZoe/Z2dkHN+Eog95P7s+V7TgyKQmBnI29BU0jiVpJZpGkDUcvgWOlSkQUhQqlKRaE8SSIIIghYIviGee11aYPV85GyYaGl9khdYRgkYdutoMRiYkhhY8NNTBV757HX+8/wnaymVet2o2wre44aZbScMpPNtjfHKYyvQEtmPiWQ47d7zMD79+I/v27SJfnsVPnx9iZKjJ9pFQhOr/QafY2Za/fcXCeUcvnTeHgckG7QWbBzc9TqlU5MHHHuXUU87hqs9cw23/+hPyngNI6q2Uv3nX2+lub3DgQI1mpQEo9o0dZujwAMrMrsK2gkqU2e8malmWcnteoiOBYYFpg+MJghYUylDMC9yiwHRBmGDnBNrKOIvNhiCxMltTRx4cQ+BamjgFnQgcZ2bRkGYnlGUKKjVNrZ5tm6QQ2RVaw3AlO8HGmxpbZjNO28qscFoJaoEApRk4ojj9U/9IdXwcpVoolUmEEiHp7OtldGSAOE1IWg3sXIkDz/0JvzyHZmUIZHZNJtaoIMBXEdLIhL5KSNqslI5cjGNCqFwG6g3s3vksPuciTj3jTXR19VCetxgzaaKiFtg5pDbwfIGhIgjrdM2eQ6mYw/RNVFwj1XW0MvAKJYRUELbwy4sxrIS2eX30zJrFUYu7KXb7jIdQzPtEBoSzF+P29dPQgjgKsA2bRrkLJ4qxDEmeGC0EUpoZ/xHJVKLYF1tIobE9j1JXJ0GzQb6tjVz3HLQ2aCRNpBLooEqaRMh6jSaC6kSdVjWiva+dVPhMtGI2rF3Ik49tpqOvjaGBCfrm9TI9Pk2sNMsXz+WuO56ms80DFbPv0Aid3W2cdcHZ/P4Pz7Fv2mTPUJVt+6d4w0Vn8uym5xiqGQgzA7lKQ5IkCj+XZX9Ww5BgJt/ctSxsU5KoLGDeMy1KbkpfWdOREwTKJGimaLedH97yGI8/sYlvfvZsDhwe4Rvf/gVz581nbt7jqmtuZHQ6YuWCpaw/92Qev+8R3vGuy9iy+TlM1ydpTdLekeNPjx9iTq7BeOhRKmTh7jo1WLBoNg8++CD1KUmiU4SSpDolSEBpie3nsEWCZ1pUmoKOUoiQHuPPTmOkU+jMiYdnaby8wPM0vp3NyCsBxLGmmId6JKkG2ewvTTRRrOjyNXPzAscVuJ5AuJBG4HkCaQhszyG1HcbrEdWqoNnMvNNKCUIl6CnC8BEYmdI4dpbTEitJNIMEnAogTOHwuMAQmch7upUydiTlpM9/mm//8E7WrF7J3J4SG05bwrbth3n+macxDEWjMc6ZZ7yBY1e/ji3bnmVw8DDz58/lJ7f9kCve/bf85Be3cOrx65huBozXGleFif7/VxQX9HXqN52w4uhqK8bWIdt3bOWkdacxWptiaGQUoS3OP3MdX/7ER/nBLT/hqd/9grbZC+nunUVbvo15He2s2XA0mzbvx5eCD7z/Cm75wQ10SI0voNOFKIZuX+PYAl9APVaEGoJmtjFXLfA9gW0KYpG1/qbI4BCuJyDNTqCCn3mf221JGGUGxmYkSFJJqrLvFbWgnmikIVFJtkHL5TNF/nQ9U90LE4outOoCe+aDZEsIIkFfGXxb0F7QeLagbXYv8zdeSN4xsU0bpJXNohoVrM5+4lbC1MAgIo2ZGB9Ba4fakYPZBy4KIAYtbXxDIqMGEgulFVIYFFRMLp+gIqgog8LJ5/D6U9fi5vqoRzGm49Je6MJ0psjPPYaUFIeQYtmCVCEMg+bUNKlp4Xg5Rg4P0r9kJWkYEakUaZcgmGByci9+oYyQglQbjI+10I2IYnuBfEcP5a4+ir5LqzKJZZnYbR1MW5JKlDJm2tRSk6Lr0ClSRBoSYmAg2BK7BFpkc0fDYvKVV1h0+kZynX0M7t3L1gfvpFSeh5szCbSNnzaYbMbUwpAUi3q9yVGzbGKjTNAKsNu6WL+gk7vvfYbO3nYcw8HKOahIcXhwmKOWzqXVyEAFtm2yZcs+1h+3hKmpiOFGwOhEjVaa0tVTIp6sE6iElpaARS2IkYZAK8GcQmb9DGOdzYClSWIIHBRTLYFva8LQoBm5KGHTaqW0+S5OzqGt7CPsHPc9dYBdL++ks8PhzDdewEev/jrHHLOKpbNmsWrDWu787UN86/s3IgnYsX0Xc+Yt4/6fXU/vvKXM6i7TPqeNl3eM4rs2YatJvlggDTXnnHcK11/3c4p+nmaswbBoJWCIhIJtY9gJqbJxvJiFS20OvBKRy0saRyYoWBlJScosMjgNQNqSRgvkDOtwsp5RmFxL0paDYi6bNwaJoKVBxDNX7Kb+C+s0FQa2bXJwuEEaCXQEFoJGKMi50Ghp4kgSJGAbgkgJPKHpKUE1/LelRsERODOYuUMNQV8btMrzOOsz38SII5YtX8aZSwsYpVnc9KPf4tgRlkg58uxjXPqxT/NPX/oY577+HJ58/jmKOZ/O9n727NlNzmgQhSHlYoGj58/+4isDY1f9T+Tt/7YoSimZ3VnW65bOZ3xsHElKmCQs6u+ibU4XtijhWwrfajJ74hVuvP4itj27gz2Do/imRuoYM0o57uyTUUeGmRys8dL+g5xz5jp+cffvsCcn6BKZLKa/LGilkvE6tGJYVBboVJDPZ27VaiyJYp1JFhJB3oIZ6yVFNxNw53xBkAiOVDLtYi3M0v08S+IIjetlbbvpC4JQ4EqN62QF1jIzoWikJflS5r1MyDpFz5PkihmUwpUw1QJbZN1chKZjw0YWH7Ma05AkKFKRIf6bzQZhs06hXGDo1VfY+tgmvIJLZfgIzaCJIQyEliiVDZXtpImfQsyffbKCXquJk2gahbl0XPJJ5s1ZSMPrxcw52LaDkBbbf/81nvrX37LjycdZ9brXEbVCKpMBQZRioQijiOqRQWpjVVABg7v2IPMOtrApFfM0Q4M0ajJ68BCmY1E9UiFE0qhWiGs10jShFcaYlovtOrgm2XIrEbi+gykVU1HCgcRAuD6TRp6CUMS2xda6QiqFaRiINML2bHLlNvZvfZ4nbruVMGiwe+ce5s0uI5MAnSqacUx1ukbQaiG9HKWiS970QGn2TidYs7s4f91RPP/0TvYfGGLBgrk0xkcxTItGFNDW1UVnZ4nxoQkKBYcf37mJ91/2eu6763GaUUJ7zmLTtgHOO3UBY0MTuKZFNYoxDBehUhJpMr8kaQmfetAiTDMAgiTBFgbVMMWSYoYlGDJSBSuRmFpQcmxUMyGsNqAeE1bqnL3xQtasPp47bvwajVjQNGKe2rSFG2++ibEjA3zps1ez/tRTWL5iBVf+7Ud46+UfoL27zL/88w+Zv3oVWkss2yTnOgjTZsHcxTx+zy8IgxDTCIhisIyUBIsF3XmSMEYVyphuTE+nRxxB3i/Q0xihGmaZ6ZaVgZr/XAiDWJOmglJe0l02yfmC9oKimJMUXMl0S9CINEUn0xcGQZYWaJhZo2ED9VrERJSFZBUcZubPkPPAcwRdZbBFNnJyHagHmVFjRV8mHA9CaLXA0IL+PCzsEtSGNf0fv5IOw2bvoWGO7e9g4VFFxusWd9/xK6xkmmajgmsmnLqsnzO6Wnz5tgc4bmEvdnk2l715Iw8+/HuKfo6oFWIJTb3VZN7svi+OTleviuPkf18UhRAsndurF3QWCMIWx7RZ1KMUPRNA/+iTz/Gut57PRGizNKdRjs2t9+xi//AQ+XI7aRhQkFAzujl5/UnoYhsrF5ucdUqJgVtuprD2TFr33sv8LkFiZqJN24R5ZSjnBeOt7A1PEk0pJ5jdC7lcJvOwbEj/P87eO8iyqzz7/a2149knd+6enGekmdFIMxoFJAGSCCIHG+RrMNkGbIzv5wz4ojG+zp8NNsZEg8FgTBCIJAQKIDFKaGakkSbP9PR0DqdPPmfHtdf9Yw+3fL+qr8q+p6q76lR1/3PCu971vs/ze0xNgqCaz6w7rtQkoSAJMjdOL4CRQqaF9CyBa0OUSBxT44eZL1npTBagZUYAkUIQB5mP2RIayxbk7EyP5VgSYWryVYM0BswsarXfheve+tsUJ9YRWza2loSdLqHvI+wcgd/FzFVQfszUxTN06y20Vkgdo0NNKgQkKZqUnEpwVJhtlJVA2pr1hYD4itsxb3gdVneBfreJcvNEzTaiMEr3+Ld55sgk0i4S9xNOPv4wju6TioSV2UVOHDuBiBL6tQ7NxhLEgnzBozU3y+riOYpDm6hNz9JdbeIOVOg0VrFI6LXb+LHCKg/TXVnA93u4novQJtIwSWMfK29RqAwytvsqioNj9Bo1FjoBfQl1H1Irx2yvi2NameZSC6QWCM+mdvE8Pc9laHwDMzOTmWRqIM/Cco2CZ9Fp+YhU4qIZHMxh5QewbAPZ69JLYWquzsEb9jMwUuHkM8+xdudWgvlVRJjghwGm7TBQybO4UGek4JAXBqlURFFMHKe0wgTDtLD8FvVEkurMNmrYKWEI1VKelf7lw0moy527Jkgl7a6i4BgYUjBQlCx3NX434m2/9R5st8DQxBhGovC7NW654To2jI0y0+ny7NGnaczN8fLXvIm3v+sNXDr+LJ/8s7voRqv8n7/3ET70/l/h+v23MLF9G889fRyrfRFnbBPTM02KRTvL4NEwt7CMbaRcevwRQsPBNBR+bFA0U4qyj1PdRhKHiEKHSj6lVc+aAd2tg1J4tiBnp5QGJK4naYea1a7EM7KbWxwpbAOKnkkqJF1fYxoGqx1NvZdlJQ0WMx2i62ZEqQSoDudwA0U3hmYCQZqJuXOFDK5SsiFUmlJO0OlC2QMRa5ZbghFP0+gJchI2DgrKhuZcTTDlSza9+o28fmyel7/yWtbtXEekh5mcqvPY4z8jWFlAGxFOcZAf//RRnrzU5MCGcbyRjRilUUqqzZHnnkNJiwEHVKrYmDOphwkR1l2pSr4URHHjf29d+V+6xHfedq1eaTZZU8yuMWuKNg8vh5m2zTKQcURhzUZ2TQwyaPQ48vNnuOPN7+LIs8/x3ne/k4l8g9pXP0e+38McKLGwGnLeGuKplsPffuFuvLBPXsKAFCRKM2gIxoqCAUsz6kEzyDzRpglaGti2QqYZUDZKwbahG4GlMolOyRX0QlCJJjAg6mexp8SCVg8GC5okzfzTQ/nspAtDUEpSymcJeVGksazMEA+wUhOsqaQU8gJHZh7vViCI0VRHNOboVRz4nf9JScRop0Qc1WguN4ksk269B5ZLv7HM1MVphsa38OP/+DRoQapion54+bqWIKKUchpiJ4pUJEgMAmmy7fmvhnKFcr7I7NkjFLdfh10qgQoRYcLikXu4dHGRqBsRqJRIJQRhiiVTRocGGBlw6SobTyQUpMNkGGLYJUpentRQDJclXmmI6bMXKBWGSC1w8jA3OUu1WCGMUqpjEzQbc6wsLLBh21UMjo/ihw06K6u4OZvuyjLG8EZypsuJp35Gq7ZKIkzWlPK0/AApdNZaWCbYHuvLgogU2wYAACAASURBVMenV7n6lpuoX1yk3VygUWtyw60vwPVsJNBabWPbFgNjVfYMSWR5BO2WWK7V6aQWqR/R6rRRS5PkyoMEnTYDawfwV1aZO3kJK++yaftGGvU2D/70GE7J4y2vvI7v/+gJEiFQWMwFEe/e6XKxFTLX1sQaWrFBVyvWV0qUrYiZZsRCSzFUypFqPxuN+Cna1GyqCtpKMrXY5TV3vJy+H9NrdpGWZsOubfR7IVOzlxhVile9739w8NqrePC793H48MN8/wf3UihCrxdx3a7tYBnouMeff+YbTE8tsbJ0lr033sxX//Ff6JoD7L/patLIpFyyGBio8Jk//whHf3YYp5wnTgVhlOCaFq70CZBU3AHW71UU7DytpRShUogkzunjDA1lhS2WAmFJWu3Maz6UF9R9CCJNOZfN94SGkcue6HNtzZgniGKNZwqkBa4BxSJEsYlXLXL6TIN+DFVXI3Wm0HAsTbcPa4YzxmejmxXSFEG3l2IKQbsvcSzNzhHNxjI8PgXtPqz2NbWRtex47Yt4Ub7OWLfBjkGTvCvop2XCX3sfzz09xX987YcUpeCpJx/m4A03c6ERobuLzNXqmEqQ0wmzScI7xmyO9DUIk3YEl1o+z56bEv+lTlFrjbBzd712+ygr7RajOYPJTsy+qkUzSAhSTa3T5Zdu3EPJLmGURnj/oX9gbanJXtmi8cAXOfOVr1It5ll2Ctw91edjR5b46L3Huf/wk1RVTEkKPCQ784IBAQM5gUw0cxE814EgzPJHTJk5UyCzHwWXf5IEyk6W4mchCKKMo9jqZRYl9xebtFwmufFyglQI0kTgx5mvWgADhQwpFsWZH1NKgWNmG7WJdRBrg36Q4qvMe9ruasZHBGYfun3F1pe8Bq0UPZUgQ0EvSEiFg99pEiYp7cUZmiurnP75YcxchV5rFZlmuTcqSUAb5CS4cUgkBQqZgSVKVcTgCJ6wWJ29iFsdQ9kOstdCqBjVjUnqR2kudwmURmsLzzQxDUgwaXYjFuo9okChkgiSDlVbs7UYAwmzU8sstn1cqQiTiGarS9TpIxxBtVRidjnL923UOxRL45SGypw//SypSug2OxTHh1m+WEMliqS9wuJynbGJdURo8HvUez0wJKbt4icJOVuiXYfq4BhnZxd5y1vfwtHDD9MPIrxcHr9Vp1QaZGF+HiUFBQ1rNo1S9Rzq9jBGGOCYKYFdoNtostrpUPRMuo0GfqS5dHySWJgIkYK2mDo3x5gXc8ftVxG3e5RHKpyfqiGFxhAxQai4/ZdeydQzp7BLFu3URJFSdgxq/Q5BmHnxrxx18JyAKLGIUYhAkqaa5b5BHKSUsQlThUGApk8QdEiXF9h2zUF+/R1vYe3VV/PkPV/nI3/wexz+6cPs3L2Zb373+/z27/0xb3vTr3P/kZ+zstLiisp6nvfKO1CdC9zzT//M/pfewYVHH2X9zbdw4eQclYoHQuIKxfv++A+ptc5w+GencE2HnAVKC3qxwhaauWad6/eO0gkUcaCwRLYBNnIuUtmElsYVMXGisUT2vWr2NEVLs9oXmYhaCCxb0w0F7S5sqEosDZHONtiWmfFKdSIw8lYmWm+nVPIS1xYMliXSgG4i8QqC8WqmCS4Vs9m/H2lKeZM00aytCio5TRDAQxeyYLXTPiwlEu23OP/Y03z7eIsLQK5YRukEo7nM7Bc+x1i+yZv27+KFr38NG/e/gqkT53nv62/mhw/8lHo/xpGCgpny+hGH76+GbHAlrVCxd3SQe5+bElEU/ddnivO11qGXXLH+ruWej9SwpSh5dDHkQNXiQj/BcSzO3neYf/rRfWxJT/ODv/sTRpcvYK6uMFIqcMQ3eds3T/G395/m8IlpLs7X8VRElGaWrz4QaMFSoKnFYEQaC6h6gq152DUuKBRNJJrW5SgBR0IsBI6dWedQmZXIKxqUyxkdBwS9OFvCIAVKCsJI0OyD0Nm1OJ/X+CGs+IJmoPEDQaQElp29cRgCS2YRBiUXygVN3pEoQ+AVNcIWRBKWrFFueuNbCBuLhLhIA0RpLZd+fi8DV93MiUcfpzw4Sr+5xMp8HbQijn0SPyRNNSkpaRgzrPv0sUgwsDT0pWTtVdcjnSKkPtLxMHM5ajMXMYM6UplMnThMJddleqaPH2sMYpSWeJaVnRiujWGYxFFEN4qo+5KZVZ9jl/qsKZsc3JJnarHP2ckl9u7byeLCKk4lR9DyCURKGiUsz14iVyqzNHeeTmOZ7Vdez8WzZwgihQ4CtOcS9vo0GisMDo9Sq9eoFvK4pRI5mTm+e902ZtLn4CveiZ04zJ46RctMedUrb+Zn9/6EIIxJVIrEIFaKhMy+NjZeZazk4HgOrVCQBj067R6eIel3u6RxShRF+I0unuVRGq5Cr42SGaQ3FAb7hkPOPDvFqYU2z56e5WP/+B6+84OnmKh6OKRMdkN279rAo8eXKOcNVBLhp2AKk9V2yrphh8RICBPI2ZpWKFCxwjEN8iIl7+ZYs6VKPwpZODfNvhtv4d0f+CD7XvRyjjzyMP/+6c/wic/+G6PmKN5okT3DOd7151/lkUee4MKZaU6cOsudr7uT1FU8ePRpGkd+wue+fB8Hr9+HsIpYWpKKPo89do6JkQrlkkelXGRpucE1N1/F5z/6GYaLA5SciFhnc9tUa0gVIoAN2z1OPuOzZ1cZx4v5+ek+cp3LjVdUWZmsI3KShTZIpRnIZzBi18huRVYhk+3YRibw7vQ1S4HAvTwTVDpjiFZdiH2FJxWRzm59JUsT6oxjkLNT6g1YOyLQholZyZOmFjkTHKFwTEGUpCSRoOwKSqZmpiMY8LIiqZF4Al630+HBSyt84oEzfPKRKY7WY154/S42GAKjt0Tr8A8xnnuIN3zsAyyvwL/962fJeQU8qVljCY53YvYPWqyEEsdxOLHU5fTswqH/2kzxcjP5y9fv1T2VsrlSoBtFLPgpgxZcDCUVFDOLdX7nH97Htgc+z/LRZ7jt1ddQ6fv82QOnecc3nxPfOT5/yCa9a+eaAjqI+L1bR7j/XJ9nH3orX/72aeJAEaAJEORllkPhGiC0JNSaRk9jpJKCl1IuQqsryLsi25QJqOayD2rekuS8FNuU6DSjc8cRDOQFeVOTRAJpaEhBXCbzJHGGG4tD2FzWbBzKTsudA4KpVWj60I8ykatpQaoESy2NcAU528LXLo264M7P3kN3cZ5YZye1gaTbWcVvtjn9k/vZce1Bjj70CMqy6dTmUYnCb7fRpsTAwrQtckmIIWy0yOIGEp2SGxrBHhxDWnnioEuuNEyn1UT3O9SW5on8Ht1ukw2FlFPzIY4hKDk2YZpiIMEQRGGIKRSmJZAIcpaJZRl4jmCmEXFuvstI2aACPH12nspQkeXpWRwvR2OpRqfdIz8wSqdew3AshLSYPneR0vAwxfwA85emcXImjXpErjBIfXWV0mCFdnMVlcSEPZ8oVgyUi2y/6oXMnzvGmdMnMquhBCNoc3pyCUMLpGWRpimkmjSEwcECAyNVJkYr1NI8RdFDIen5mQo46vboNFZIMEmSiH7Ux0ojklhlp6EpKKIIdMK1FZdjqzEl1+Q/vvEYb73zJq4oCpbbPZ55Zpr3/+5rOP3EWaJOB7dgoOOICIkfCnpByvrhPFKmxEowV9fkbIe8JYlEgiUdtuy7lvf90Qd5xZvewUNPHOaTH/sHzj10P88+9xiTF5f5i7+8ixteeR3f+eQnefMf/A3nTjxJffJHVMfHGR3bxunTp9gwOMKJk4dJtM3+A3t482//Cc/8/Ajrt2/hPz7/FXbu2MpSJ2XDhmqWfGhIRibG+flTj9FZ6WOiSIWB0hFKKRzL4NzUCi999Q6ePrrCxFAOQxoMDVq8+cNXcuwfngRT46rMHrvSE5gSTJkJtpHZVVeHmR/ej6CXZA1C3YeqlxUNQ4KZF/TDzELY7GUb6bwrKeUz/70jshtW0QRLpPR7EZKEVGkSQKWC0ZzmyZnMinu6BvVAUPMFdZVBbG992RDn82vJqw4TgzlmVgKarYCPPzbFl5+eZ2tBcvUrtjNw5Vr49BcYffEuvvbQMiUR4KaaWEpKOZMVXxBJky2FHNORQCh1V7PvH/rfFsX/TAaeqOb11i0b2blnD9bceWxpIGVKogVCKfJJRGX9KHcdrJKenGFoe4F/+sJh7vjCMfHwdPuQ58jNbz44Un/FFTm2j9ocXbE4XQs48/Vf4fc/9F3OTHfYsWstKgyII4VA0tIZRqyhwBOSnCNxPU3VgulG5pNcaWcJdY6RvZmJFrSDjLeI0tiAMCS1liAxsiIWxhkgIok0jpHlR+RMQZRkonRfCWo9wZqqpuZnlsGik+WkrPrZBjpOBQMFMC8TNVQ3YstvfICB0bX0W/MIwyXyQyJhQdwj0nmCTot2ahB023SWa+TKVeq1BZRKSOIAy7DpRj4kKXYaojFIUeQKebxN++iuTGGXxgiDCB11CFoBy7WLJFqSJjA27LI01aBsC5qhIDU0hhREcYwnbQKVEiiNUimFskuhYuMIC5UkSB0TKpCpZMvGIrrRYamr8DyXVNkY2sEPQ7rtJt1eC4HAyRcxczZRL2J1dZFcziKIFUHbp9f1yTkunWYTpROSOCHsBYyvH+TgC17Nw9/5Ft0g0/s5lsCPY3aurXJqchnDNLEtgzRNMuqLaRDriD2bRslXyxiRj1KKJE4xpUmnXodU0mw0qVYG8f2YnFAYSYRyXOKgRaOdLbo2lQpYhslT802qjolpSY48eo44Z6GFxDFyfOMHR3nPH7+BrVWTI8fnwHZZ9RWOsgg1XKpFrLQ0S01FAYHtSOIk4eDtL+P5r7qDtTuv4hN/83d8/5tfZ7Dg8lu/9mY++/Xv8+9f+zLv+s338+GP/BUL547hVIe44QW30KGMURhi9fgTPH74u4yMrmd+pcVrXnYH//zFf+FT//o17v723QwWC4Q6ot7scNv/8SaeeeoYUT8kSmI2b92K34+48ba1fOkTd+N5Ht0oJlaagp3denKeiaFd1u3OY3QiOiH045Tg0UVIatlMT0EcQzn3i/RRAfJyzhFZ0TJNgVYZDcezJEVbEycCO58VxSEXun6WtmmILKvdEJkBIp/PiEWeJ7hyM4gQSMBOBWmSEvQkSyuw3IFGW3OxI6lFAgyYV5ptt25i5mKTH8/0+eibN/L0AuhOk7fdPoEKI5baCpUoPv/MMg/df57NqsOmV+2Cb9/Pa373ZfzPv72PnWuLmKaFbWiKtkFFwFLqcMtN11GrrXBpqXZIa/H/JaP/505RCEHeMfmjt73xrjXrNuM4Do8vNXmxk1BLFCEpuzxBI0j5l/fuovHwWZ6aXmL/Px190bcmO28p2uKp541an5r19e/c/dHb7xoKIs4vdrnzpp2MbRrlgcPH6AuPhbkOv/u2q/nK/Rco5V06UYzrugzuvZr84jx9DQU0ttLUe4JaJ8si/gWCy3EFOSubE1ZKYLgGaQqhllhGBiZw7Yy6nURZ8xtHGYOxbGe6yJID66uSteXshJytS+JE0EpSLrUlscrwZEVHE0SZ/EcLCYmm7+S5+k3/g8bMKQzDIHRKhM0GzXZAGvjMLkwzUCkwef4ihilZXZxHxxFhFJKEfUwjhzQk0pQUEk2iE4Rh4jouKlelsTrD8J7biFdn6fkBIg6prc7Rafg4pg+qybil8aOIUJkkqSJVEUXPoBMI0jQlVHHGqxOafN6G1CRXsClVrKwrt/P0gz6NfsrzD27ixPkFkjCg53eJ/ICQhNAPMgR/HBH3eyRRRgDSCqLLLotcsUISNImDHrhlXEvSWW2w5+arGXI28/1vfpnSQAmhJUJoIq2JVUzZlVysdXENiSETLsdz4xiCsZEBSkNlIqWwcy6RMAl7TcIEoljSC2OSKERFMUG7nc2BnQJRs4bMD7Dp5tfx3JOH2TmYx1eK5UYXaRiYwsD2TJrNiIWlLl6xiOF4/PCen/LTZ5cQjkOtFSJiEykt7DQlZ0lcIwNlWJakF0TsvuE6cqaBSgWf++u/5GV33MAN+/awa+dOvvSVL/CSG3dxYqrB3V/+Iv/0uX/lLz/6KVSzxvNuv5WVSwskUcyaPbu4754fctX+fczMTlPwE6rDFb75wx9x0403s1hrsTwzzYad22nXV3nu6BkGRyuYVoRjWjhWjspQgS998StY2kBphdISmWaqBikl544H3PmuMb57T5OhjTmGA0l0+iStriCKMtiKLTOqjSGy8ZJWmTbXrkh0Jo4gDDMjRTfICEuFXKbWMCXYOYEpBMMFTSvU1HuSOM3iDkxHU7WzxMmSBBWCSDNSt6EEq91subnQsOjs3Yc5O4+yYVmamIbJmi0DHDgwzEq9z5tes5EnnmuRmg7XrUm4bd8YVw6ZnJ7t4bgm55sxnzta49h9J7lt7yiDySK3/9pNXHrkHMW8R1cLXDQFLIKRdVw5UeHGPVdSKbp3PXX64qH/NdxN/uLerLXmbz70O1qZOW4qxvjnj8PyEonncH3ZYXfJpRQp/vDGAb7zlaf55tELvOreGRFocf/OgbwOQn2gWLJ4wwFPzz9xnmdqEduHDX7yxJMc3Gnx13fPcfzYLP921wuwen2kFDzx+TeRGDbm5k1c8eK3MvKbf8pqrDnfh5OB4MyqwJAaqWAxSSnkJVpnVr4g0HR7QKxxbU3R0SghKOU1XSVBZVvqOM6wSZhwdimTAZxYEnzrjOaei7DUyvBEwxXNmpJB1dXkHFhbBq0lE4NQykksrWn7msLQJsKoS6BSat2U1A+pLc2hFEydP0tzucZK34VY0u12CYOIdqeNSMGybVKZEkZ9jESj4iDrElWKHyc0V5cx3TLtS8+ytLSCmQYs1ZZoLq5Q8FLGPcHugQq+3ydXGmGkmkNpME0PmaTkrJQo1QghQRnoFOLExCtabBxwUQIqE6N4bkKlWKDfDzg9VWPTYB4lBFGcUK6MkHOrGFJgmiYqSfEDn5QYFce0+j1aQYSIAvxeE50rkLNNrKjO4sVJ3v7776UsR/juj39ArjpAqgximYXKS8sBbTBT7+NKg1SkeEhSYeGYFqaXp5toHCNFKkUOjZNqQlnA0ilCQq9Vp9Xts9psYxkCE4Hf74Obw19eZB0LbHn+qzjjrOEnlzpE0iQxXEIEkTLRqSJfrmDky/j1Gmu2bcU1HMraYsguUJAxtqEwnRTDdlGpIu+6mKZFtVrAEiaua3Hvlz7PK3/1l0gWl5mcvUCz3WB+Zoktu6+mvtphxxXX8bcf+SCf/+RnOTnXwBEBkxfP0Eli2h2fIa3otBtYpmBycZaX3v46nnjkEfZfc4B2e4U4DenMLnHvl/6V9VsniHtdCrkiURzRai4jwjFueul1dLqrCK2xDUmks6tvqjTlXMiTD0WMD1vEfsRsr4vteJgGmKkm6WXMgDjNCmQ7kOSr4OYh6mYzfpFkwWZRlBVPx4RumM3vJ4YNtq8RzPc0hbJBxTVYX4bxIuQKAi9Tr9Fo6ozmnWrCMEMCdk3BNqGZsjbR/+DHkHvv4OENG9lrwCt+ZR9rD6xjcb7B+95+ENcu8Prf/AmLqz3ecNCjJUs8cnaJazbludjW/PoukzDVSMPgezW483PHeexr59n8vcOs2TXBZjNhg2PwkqE8D7djahdOsdVQFOtzvOX51/H65+/T+jLK7xfRlwaA4zrc+8k/0W9wNMbKJP/+8FEWFuZYaXX562eWt7z86u3vf57oMm0IrFab1U6ffzzZZSpID+U0XPjI0F0vefnVd91Q6fGT04IoUtz5wiuYaYYMjRQ4N9fnDQfX8th0yInZHivLKxyf6fOH79nL5+7rImTIbS95O1F7Fe+6F7OaBNQuzbBJpmgp6KSCIRdmOlmWRBwLokQwmMuoN0pLun5mO/JjSRwIBJIgTMmZIF2BUmCb2QmXt2FLVbChBOHlUFglNP1eipPL8GSdJFvm+CH4cUorkPRjjSgXGd99Nb0oJA5D2ssLNJshRsFhebXPyNAAUxcn6dUXIU7o1ReJYp8kAUNK4jhECBPt97CkxECgRRYUHiuNlfcIOl28kfVEfpfZyTkGyxb7tw5i+oqR0SrtdoeCY5I3I/KewXS9h6mz16afalIl0KRIYZKkIWkAqe1iGJKg3UJ4eYoFF0sI5he7bBkSnF4IKHgeQeIzvnEzM5OnSA2T8cESuZwHWjBUtnC0gVbqFyHSmWdVJywu13j3Xb/L5IUFvvHVrzI2WMZSJlJGxMrIOkJhEqmAKNEZVNU0KVoGbWVgGJKibVCuDjNUtHHyBcK4j9YJdq5IkChUqGg2G1ha0g8iLCPNgAVaYNoOMvY5d+ESjplDejk27b2GdVt30ug2wcxh5j1GhwcZ27Kb08eeYN32nbiFIpZpYLoO5eowWzeupzgyQRCH2CRI06Afhhgyey3HJzYQqy4nz81x9bYJer2IXD5PujrFz0/M0+21ufHGO7h08RSmUHi6w5kLlzh838O86Tdfz/qyRVo7y5d/+DT7912N78e0Om2mps4yNDTMucnTlCoTtFcaGGbKhZMn2Hv1Ncws1RgYrFDI58gVS2gluHJXgS9/4TvkPA8lYkRqICVESqNzmsa5Pu/4yB5OPNGgOmQTxwZl3cYrZresnCUYLGiKrsC2LltgRcYfNYTAcwTViqDgSjxHUPJSBjxJrmBgF0w2bKnyoxM+ti0YrhoYtiRNIQlTbEuwrgqOKZitg4gk8z6IlmYxGOQzm1/O4EvfyBrDQUVNFhdnOOInXLc5Wxh+79E5luYW8RyL4dEhbpkIuLAaMzExhq0Nwm6Lrz/T47eu17z2jTfyoR11Hl0QPL4cM2gkDDuScVvxbGCw15H8NHD5+pMnhWtw1zefPEFfadzuKh969YsobJu464FHnz0k/nNR/NN3vE7L0yf41IOP8Vc/fJKjs7WvH55r7z7f6B9SYdCYKMi7HKFZWG0g0Ty+GnHvUiAGpKgP5sRf3vWDDoPzM+zdtwk5McDHfzDJK/eWkZbAtXPsuGoTz5yY47Zrd1DyUu59eJIJT/L57y0QFEqYaEynQK44QMkwGdl2Letf/GpWWm1WLk0hhEZrga81W8qCahnGxzzmVhUizUCpQxWJkiaBnxLHKY1OCpe/MDrNZiGOmQlLm37WQbYVlN2M4pF3Mh91kmRCY1dkNkBtCrhsCxwowNDzXkixOkbj0jRxrwV2jqVnn8GsjHP6yYcwLRtLQmNlBYUmaneJwhADTZIoFCkiTXHsHCJSKJFBZlNpom0HGYPh2HTby3TaEQOeyW03rkVYHmNDgxy8aQ+dlSaluI0fdik4grINvUjQVTF2KnEdCFONThNUalAdqaJaHUbXjhMmkmpestTuMVh0WO2FbB0oIKRgudkl7DUpFMvc8ivvoj2zQtmMee2LtvCzo7N0ugFj48M4wqQTKRzLxBMJcRiy/9abmZgY4TN//2nGh8cxtEBJSUyKbWTZz8I0MaQk6PdRhsCRJqZl4MeKgudgl0o4Xo41QzkCFZF3DPrYeCKm1g8whIMmolHrgEqxXBOLFJ2moFI0imatxc49uzn21DHQBqlOcXIVgiBk/Y7tmNIl9rvUVhqs330lQjp0V+cwDYeiiFHSoz5/AbtcpVVbAcPBtVxUkuCVigwOlwg6bZZbEXvWrSHVKW61wk8fe5a3/vovcX5ynk5rmR1X7Kbb8UmdEjlbsrq6wHfuvo9zx5/mwafn2JjrUlq7g1a7x5qhAbRT4Motm/nAn/8Vf/LhD3Dx7AXMJOKK591KvTbLpckWo+tG6PshEigXHMZ37OZrn/ocOStPksrLgBGBaeaQRHSDkPGxIksLPbau8zjXMCl26hSlplSUVL0UlWQWWtfOSDmek42phNJZ45Rm+C/LIYOxBOAKTRKn9DoJ7V6KY2oKTvZcJZKBMlTsrMieqWfg42ZD0296nL3uLUy99nfZViowWCxiux6P//QeZBITSZsbdpY4/MgZ8iLGdvJcvXs9144LCmvG0Fqxfc0wtYUFWpHmoTN9/ubdB3jiwQv8/SNNppY1poBjnYSXri9wvqV4thlwzUiZP/7RM3/VC+L7F1q9Q7Wuf+jC7MLho9NLv3bkuTO88LoDPPTc2bu6veDQ/1sUHzpy8tC3nrt06Kn5xqFWog/1lf76f75jPzHdONTP5e6yXJsFBX93bFWg4c5rK3+50gq4YtDiK2cV77nGwS86fOqHC1y71uLXPnWG2swcU1GJpdlLvPPNt3DxqVPs3b2OdmSw4ExgG4Kw1+Pm217DxPhmTk6dpWBLBnMOlSsOMHbnb3HpoW+zlBgMGwkzfThTF7Qaig0Vg1CnNDqCkpN1h42uoudnuHUpMqhENxCEiaDva3JWhlIPInE5lMukMlwgUCmVvIFjQhJoigWBdDSGNkj8zBbY68LIVQcprd3Cme98izDv4poWT5w4SXPmLF51mPr0OTp+j6DrI6VJr7OKZTrEqUKnClNIpGVmRO0kAWmABlOaCGGgdUIUx0ShZqWxwvU7ilx38ABr8yb7b93H+L4trNs5gkzzjAwPc3ZqiWYoiGPFSN7GTyHVJo5WhFpjC420Bc0wwRU+c6tN6s0eDiZRrJFK0OwqBvMJ040E23BZWb7E5FM/Y++NN3F2qYZbq+MVTJbbKau1Onv3Fmm3Ne1uBy0zmMCWjaN879NfwypW8SwH03Ih6WdykVQhLButMj1pPwgQhsyiMA1BrAWubWI6eaTuU64UMQybxdU2ruehIkUiPJRt01uuEaca1zCwTBPbtemHCiFMEkNTsiyePnGSA9fvYfnSHO3WMkG7CVLQqPdIel1iBL1Oi9zwWhbOnuS1v3w7y5caOL0GjSTGHh7Hki5J1GNifA3oCMu2iPyY0fUTdDs9DCzWjA+RCAlEJFozHHXYtmUtP/juA1x94BpWFk/S7cLI+BD15ZgX3fZ8vv/o04ggYs+WK9GGYMOGNRw9dYGJteMYmYW28AAAIABJREFUcYSdz7H3moP47TaNZpvtO3Zw8akHGNmxlycOH2VsbJCRkUGKA0NsW7uBB448Tm+hTtHoU3Ghn4rsQBLguhY/+8kcr/vVzdzz7Rmed9tanrooaJse01MRhZddT3FmmtiCKM2ssa6Z6WWlIIN5SIEfCQyd4b8qVdCGRAmTejNl0FV4RsYgKHkGQ2OSskwZKGksLShVbI48m2N1dDerH/oarqkZswXNVhNfKUoywi0MMXnqSQrlIj862uF5uws0fYs7Dw6xvDzDmpEC33viPA883eITP5wkny8gu02eWUl5486U//t7S8y1BfuGNSuJwR++oMSHj4VsHMnTjg3+4tgSK83ezf+5pgWKyVU/PvTscuvQVx944lDvckH8L6LDsvv22aXWoXP1fu6J+e7NOk5Z56Df/PYdPFVL+eCVIfdfSNkxCrPzcP+ZFn/66m3cusNhsdHlwHDAn97bYHlqkg++7xbueWyJH8/lyefzqKDLlTe+jIfv+3eOHn+SN960m6HhQQzTw9Apbr/Dxhf+EjtueSnfv/9eekGMqwVCp9TbCiJwXUGjCypShIZEkNIONG4OTFugk2xxki9IDNck50lM06BaFRRLBmnsY+iUpZYiDDSYmlBnp2Kvr/F1tpE2DTAHx9i8fQff+Pb3Ob24Qj7scGFqifZsA9NStPoJsR/hd7Lg3ChKsq21NkGnCCHRWmGZkjSIETpFmGbmdjFdLBM6fo8wCqhUHF66fxPj42U2jFZ49vQCP/vCv1Osltm4fZjVWo/+wjxLnQTLsxh2IoJY042SjH8nDJI0zfKqpSTQNqNDJTy7QM7UaCWRUrPS9BkaG6EiFLUgxXVLpLbF4uw0XpLwdNvmtq0O5xZ7aB3TWFXs3lWh19cECmI/Yte+7Tzz3BlyXilLVIwitMxiVU2REsQRAvBEiiM1XSUo2ib9yyJiSIiVYGSkSqVSpt3LXCQyQxghVYJNzFzNJwmj7A0RCYbpYHsOnVYDK4yJDAutJe1exJot48xduIRhgk4DIEBFCSqIaHVWibodXFNz6qnTbBkuUd25n9bCIl5liKWLZxhfu55ep4mdy9HvhKhUM7a2QhKk1Fo1dm3fRqO2jG05bBnKUYtsao0Om3ZsQOaKJP2UVrfLxo3jPHb4AQ7c+gr2X3Ulx449yfnFGm/55dv467//BLuHTM48e5T8yBa2bN3GY/d8gy3XXEttoU67scSzxy9y3ctu5fiRS4ysLZN3bUwzYnlxlergEKcfvAfbMklJMNH0lIVWEalOQcJVG8oURxxOP9uhOlDkYk3xvrsO0n36OHG3jzSzMtBPTMJY47lk8QIItJnN3DUat5DZ8RxL41gphrRYt8mjWlSMDCmKOYtcosmJFE8InjqnmfQOYLzv45SuvhGvuYjl5kiTgJfu38ndX/sM09NT9PwOlaFROu0GA9UyT092+PXrDdphnyvW5fmNz06yf+cYb3/hONu9FlcOpzw9m3BqOeHgiMmnj/rcuAYemdb8y+9t5M++ushUM+RMLebJ2aZotvuH+G88jP/OH/uxuj9KNNvKQmsJjz+8yB+95Rb+4D6fd+6MmFvo4rmC+y8EvHxzQkrKwd3rSVVEIzD4+ek2Dx6d5jtnPcZGS8QqYnjNVpq1efrtJoVShcLEDtoLF3ntS27CsByqRQ9DC0yRcvbYExj9BtKEoZLJSMnAcGGpl1Gz+0bGYxRKIs0sX8bR4NpZtGmaQjmXYkiNiFOEzoKdtIJOCEvNDHhQsAVhH1xLk89Lyl42jA4C2PiC25COy48PP4aKFCMDkunZVbpGjIrSLHJAaVw3R6/fQRoOSpOxY7QiTVOENtAmGFqDtNCAtDxAsz1dIlUJjVCyc9s69qypcuD2a7jnSJfzzT6N2OJH33iKwpphrthW5qdPLjBb62HbLn0krTiPEA7aBJOEWAuSOAFhYKQKv5PQ73VJ+orUUBimhWdqZufrVEoWpJqu38Uzc4DEiiOkTkiClI0TeRY6kjAISVLBSKlAHMU0CNm3bYRTJ2axXZc4SXEdmzDoZy4dqQADYRg4jkvRktR7Pq6QGLZJpAU6TSlUBli7bohWPwJM/FhjiZQwgq4f0E8lhAn9bgspwCt4dEJNu9fHlCZhlOJIRawVS/PzVAer2KZB0OkR9iOiro9hO4wO5GgutfFkFtpVrRYpbL6KKO6zducVLEyeZd1V16BSjSFT+r2AYtGjUW8xsX6ETi8hjWO27NjD7IUzbFy/nsm5VZQwKFZy1GsdXE/SXF7CLRUoJl2eOX2RzduvIO7UWa43uen26/jOt+/HBl755t8gThO6nQYDtuBT3/wer33Fy5lfqpFGXWzPRbZrnDs1xej6dYRhyK4du+n3Y8ojBf7t019iqOSRaoHQGRwlvXwA26bgseNtNo9V2bQGlla6DJQcylfaLH3nNMIShH4WJcBlFUAYa9JfwJjjLCDetgQq0AgjG0PZhsQsuyQyh6cjNhYUuYLJwJCHaaZ0exohNGfGb2b7FXtxbRsv77Fvx0byRsyPf/Q91u3cT2t1icbyDMXBtaigSxxFFEpF7nmyxsuuzfOuf55EJim/etMooR+SKMHt+8f4+mOrYBhsLyjCRDGUk/zp+/dz973TzNQiNuckFzqRiJXmv/uQ/91/EGhOt7T4+9eVGB20WXnwPn773XtIUvjKacHvvGcNgzmD93xjle8912P91mGGi3m2rSlw5YjmqRWX9RNlkijBNk1yOY/a4jSvvfNt/PNffIBD7/1Vco7kD/6vD3PL1RvZMxDjzx3jG1/4G/pzkwhDIFLwg5T5pmKxpYmVouuYTLZgJZBoqXEtgc6OO5TQFCtZaE4qBYGvafiaVBkkATT70OgJKrkMsulHGRk4QRIEmnozS6ByTJg+9RyEPlpDEimC1KAXpZRyw5k8JkkQaJRpIDExBQilUGEPEBlzUStSPyQOk2wehkmEYEB1qdJib66L7PtsGS2zdddavvjpB3nxr76AXDvg3Nl5RnZt4PMf/ya5LVu5bsNaUpFQzSk8baMwSKMuOrVQho0jsg9wmkT0/BA/jkhCRScIUYGi40eZn9wymV6JGay4eI5LFEWg+rhujjyax2Z8xgo2g26EYdssL7dpxxEVx2T72ATzZy8hvAJSSBypiKKIVCm0SqhaAkNH5AxJ0m2iE0XZuJx3KcDEwDZMorBDp+0TBzE9P0AaEj+R1Ftdom6DXi+g3fUxHRfTzLI9Cp5NPufR6fXwjIBWPyYOEip5h4WTJ5k+dY6B9eOUBobI5V2WF5a55VV3sKxj3JxDreNTKY9gWCYqqHH1ra/hqpe/Ei9fxEKQBAqZCvq9CMsSRFFGmxZWnjAKcarj5AeqBHFKr9uhm2jWr1/P0sIqTqFCKkxarT5b1ozRWphhevoim9eNcmm6iec4LNW7XJi8QBRaRCqhHyp2b9rAzMWLqNDHTySj67Yy0/JxqwXanQ6DI2PMLs7RbcwxsW4NA9UhUsMgiAW1AGKtkdIi1VlwmWfE3P/IBe7+cYu16zz2HvD4j4/P4TgCkaTZ9lqmmFEWfLWwmjUAcZJmQA8ro1GVhwXFYYF0BZYL692Ai/MrtP2Eo1MCpxXSmGuzMB2x2NE0lWDxvq/x8P3fwKyf4dX7RumuTvHxT3yUN7zujfzRu+7kbW99F45psjL9DBt3Po846qCUYnTdGH/0xSXu2AgBsGHE5fotJVaaMa/62EUWugbvuH2Ic4HJck3jXneAD/3rSWYXe7x6IxzpaZHy/+/x3y6K+vLvV32xLX7/gODrk5rGV++l9N5XcF3Z5DXvPc+FH7wKK3X5wfEW2975IB/89hx/8vaD/OiSZO3ajYRJTKe2wMSGPUxfeA7btDj52L2cOfo4nXPfohuGhH6XYniRS488xJNPH+X0M0cJpCDVktVUcN7XnOyldAo2r3r3izlp5bj11dfy6JJioGwRBil5N7s+y1QiQk3Bg2ZdEymB7QjaKqWjsgXAQD5DHaWmgdYpYSKIQ7BLMDgqMXJZt5j2AgY3bWbN+EYUmr7KqCn9qI+JgcQgSRJUr4shDUgNLMtEmL9AhUVIYeIKNzNc6wSERiQxG80akZK0+glrJgqsGxukUBLM9Ewe+Mw3MYjZf+BK+o0WE0WPL33qftbuHGLrqIc3OEZ1uMjmQge3aKHSBENaYBukUqLQaCFIVIqd85BC0o4S+j2fIIwoOAbFnKSfOAxXcvh9Hz+ApXYTLSQjJZenLrS4addWIEUbBtOzdfraZGh0gIXlBOv/oe49oy27yjPdZ86Vdz5hnxzrnDqVk0pVylkoIUAyCBswwmCCUzu0m7bdNhhuN3YbZxtjgsFgcpSQhECAAighVamSKtepcHLcOa08749d7nFv/2ybMdzz9x77xx5rffub3/e+72NbKBGRNEFEHulkCmkouh0LJ27/AfR0pak2mhhCx5QQoWGZBqHyiRsejVaAW28SRYI4UBiaAcogDAX4EXGs49UaKCTZhIGHxFABuWyOViAwkyae8qjVPSqhwsilOHvsDAuL89TdkK6+Tv7hY5/jaw9/nKZucN/b7mG5XGDD7imue+DXWX/pCZ774uc49fTjNCplkl0ZhK4jpEJF7WffsGx8zyWMFAYx1ZpOMtcFysCvGVyYW8f0QyrlIltGMxydXmbTpikuzl8ibnkkbIdXfvoSb3zTbVS8JpfOnEGmsySyYwjDgs5hgtZ6OzzWTKOESeP8Cd74ljcTx4pU1wCWncLuGCBuKG5+y/2sVGJqgUEsNEwRXxbXKqSQCL0NJ4vdOo98+wLf+NIq996Upex0EbUUgQuNuiBWEZ6vSNvtSLAgEEQxtFyBpYEdQWEO3BrMLCiOXgjJ6jb65m2sTGzjDw4lOVc1MG3JbEExvQqRHnPhyUc4fuokKwefwfaLhGHIwvmjnH7+axz7ycMYWoyGxamDjzG+5VqK6wsgJV1jYzy1kuK+vXkWSjFv/ptTvHCsxMWVBr/39l38/HWjfONAjV9+yyasE2eZooWuK750qd04/X8r1s/s+vy/n6+cUx/+xK3JDz26DKPzi2z+9XsJDp7gwoHzfOmj+zl+cJ0Ndkgc+vzul86zYfMm0Ax8t87ua17D6tJFNE0j1zPEpF5gV/k8s26NL/zgGB/57XeQrdZ58fQ03/7+E9Tr9TY5LFbEcdtfeev143zwt+/CMRTveP1VrNcadOUzfPWFJZSm0YgVfggNT1D12p5Ny5bUPGh44IVtfaYXKmq+oum1N29ucFmkailcV7Q/X1dUW5La6jLdm3fSkoILp86iaZJG0wMFli6RIiaMIizTJooVQtcQUkcgEPKy3F+FqNADJZFoxEIjGTfoM6u0IkkQKXJX7Wf7UBZhZjl0aol0o8a2O27ie19/DGEZWKZOtbzCrXffyJnTK5w6v0IqYdLRlWR5vYnSDWJ0lIrwvAhd0wn8+DK/N0ZoEAaKPfs2sLRYQwpBtVpHJBKMTPSx+9o9TE0Okk6mWCmtI4kpeRY9To1CTV3WtymqhSrZrg4W5uexDbPtWMmnma24WI7BcEJSKjfJ6m3AlpVIEHkhhhSECHzdxNJ0YqkR+S5CakS+Rybfi25A5CmqtSpSt1lfWqNRa2CaGrYmkZZOGLbtYG6jRYAkcFtEQYDn+ZhGG/Bl6BAHIZGCeqVGFHh8/nMPs2/vdn78oxe5ct9WJq+6laiyzpc++8/c9OYHSNkJls4eAacfRUTc8gnDED1pIZVPo1gl3z9A5+BGDjz+L+y9+R4KhRK6lMi4iasEsZBcu2MX33jscW7Yu4eiG+KHLpbUaIU+uYFN9CYtnj3wCrffeAO18gpnFgtMDea5dPocA5u3UVgpglchSnWTHRph9eJZSuurZDM2HRkLpQS5XI5Hv/kQjtP2UUYxCCKE0P5XYZCiLdcxbJOImFyPRdqKUMUKhtVG5oaRxNIE1RakncvC6wTYEtIZcH2w7PassdNW/OSc5ME37+GCn2B5tcDkeIIls5dHXl5D92GpBq0YiqUWsSEY6eli89gk8w2Xgy8+Q59XoVJYZKnu4mTy2HYC3dBJ20lagYdjGm2I1VKBmXOLdFkhc03Bjz+yGW9piQf/8jQ/f3039b40mbkFTqzBui+4UEH8nxTD/+NO8f/XNcaK+x5riDemA753fJ2v/uHX2H/HKB8/EfMPnz5Bbucwf/3BW1jWkqR7+3FSaVzfY3xsFLfVIt3RTaarH1NolHPj/POq5AfrvTz0qf+HTmJeOniQ4ydPETSLmELQUALpWFy1o4tUPkndbXDm9FmOnZmnsLZEpdKgNxNgWDrlWNAIJUhBOikZHRRkHQ0pYjKOoDMBfSnIJtrBtSlT0JUUJGxJ2lGEkSC6jB1ICEVaF4z3w1A2plEusHP3dtaqJXy/HYDbBs2LNoRU6kB7fhi5brsYCkkcRv+LvSx1G0QMIiRUAeOU24b8OMJN57lpJIepBSxNL5HvtSi6VT76/g/TOTyEEwUUqw0QGYpVgabrZDM5jl2qUdNMLMMkbvmo2GuzkEUbp6prbXlFGMeoEHQtBh9kHDFbrPHA2+/kbz7xG7ztbVdzzc4+HDuidyzD6+6/gbHJQRzV4vBCzO/fu526F2IZFtLWsIRENywwNXTdwpYKP2rjJfp0i0YQUooCHMtkcbWAIUAJrf17xTF+rGHF7fj/ZqmKF0G1VOHMkbOUGgFhPcD1Axo1n6DRQNcsEukEYaBQ9Qph4CM1hVupEodtsJSUGiEmcRAjFEhN4octdEPDMiT5jMPLz74IUcSXv/0kZnyJEy8f4O2//d8Y2zCFY2n0b9yJDGoYou0OkprWfh4SFr7QMXQNEbq4uQFszSbf3YMfhTQCC4TG6OAYX/nOI3T191OoVIhCHaU5VGstxofHmbs4Te9wL4Gn+OmBQ2ycGqF66IfsnhhiZWmJ7oQkin1W1tewI0GtuIBh2AyPb8ALW4SBolypsef6WwlEhFQmUkmEii83iu0Q4zhS7VGGalOpRODz4x+t07GhAzNpYOu0c0O19iyxv1Nh6ArbaBskqh4EQFMpCnVBI1Kcq8Db3jjMXMPFD31uu34n3bkMPR0OzvgoN739HoyOLGuuhpKC5UunmF+Y4eWXn+Ett9/CLXfez9NLLmfXPHp7RzAsm+7uPNXqOnv2X09zdaGNQujtRSbSlCKdq3Z08eV3d3H28Bpv+vQK/+W6BE9MV3ny8yd59HzMug/H1uN/U0H8txfFy13W+w4p8ek7MiwoQWp+kTuuHEClkzx/dIGDR86xEqQY6+/Ddz3yRsxV+6+nsHgagYA4xrQNogCiTD+nfvIUo4Mb0FZmmJtf4NlXXyZjGoxPDPLkp36Vlx/5Q776d7/Jia/9Dn/0K6/jsYNFto+mOX52las3Zfje8zMkHEmpEVGpRhwvKtZLIWfmYLUe02goXC8mCttFXaMNp2rVFYamkHGILgSOFpNMgi7b29GEJdpEMhe8chlL93DSeQqlCoZptedjUl5OwFHEcYyhtf3GkR+gogDNMLBsGwWYjkMcC5Sw2GJ4yNhlttBOY9n2c/eSlS6ZZIpsb5bply4Smg63338vYRCz7lnQgGv3jVCdPsvwYBbd0sg4ghPHlmkpSaSZeC0P3TKRQlGLIvxmwExljcp6Ab9Ro8sMWJhZxJUmn/rEm8mkfP7hrx7m9//wYQJVwo8VlbUGj3z5aVrVOm9+7z0Ymk0gXXYOZSkU15jcup3C8gKNICSIQmphRH/CpuTW6TA0TlWb7TAO3aIVC3JOmztiRBGaUkhpEgsfy8kQqBBBTLNaZu7UWRqVGrOHjlCvlygsruJ5DfxIw2/VCaOYWBiEZgpH6oReSDKXugw2c4k1jTDw2wCqIMYLA0wkKvbxvRBigWMlCMMQg4hyHXJJm7Xlk8ycOYDSDCqtAnrok8p00UYyBagwIAhcgtglm86xvDzLjskJ/u7j/5OEarF10wgbhga4+up9rMyf5/ArR3jL696I3rmBjZu3o0KXwvoqmUTMpYuzJJw01165iWMvvci//O3HObnmIeKQpdIqhteOz7OtLN1DPRx99oeUqms0GiUcO4uMQvoGe3n8q9+iN5FFqQCEas88hY5Aa2PfVTugAdEOW1UCgsDl7NkmzQ0bkK327cuI28+6bTl0d1goTISUJFMGi3OSetXC0qFQ11kuSP72u3McvrDCQF8njpXkB4eLvGlTDw/91g28dWeCv3j/zXztb+/nvju3EwUBTzz3EzqI0AtnuWJrHycvzkG2ox2LpmuoOCKTSPOD73+Nv/vLT9KqrBEEIX0DI0Smw7HjJX7liwU+8FiB/3FbkkemPTrrAVcOSKQpOFX510ibf1tR/Dddn//1hLHkUyeiD3/p9ckP/eNhjwHb49SFGm65yT+/HDI4PEjV9SmXSvzGu97OQ498g97xnbSaNQSQ6+gim7KYm7nAw5/9ONa5n1BdW+OzP3iBhOmzuNpg40g3X3/yOB/8m0f4xDdfJAgifu62XXSmdF44vsq2EYfnXzrMSF+CroSLXzfwNUl/QhB3duEiqbshmCatsP2A1DwoulAO2ob3ZqBw0Wm5MQ1fUKspKi6UWpKapwgjkLqkEMcMToyRyQ9w4uhxpC5RscS0dFKm2VaL0H44DctCCkkUeIhYEYSXzdRRSMbQSYUtMt4KTU8gIoW561ru2TdC4+wZDMdi8y0389d//S9sGB1iZXaRYsOlS7RYaNT5yz94Pd96fIGFs6eptEJc3yfGIFCgohipK7yqx3VXT3HrzTvYccN1/Mov3cb1t+1haGKKUEuSS6W48759LJ1fZ6msY+stOnuG+Oo/P83YUD9LS2v0DA1RXipx8sAZhke6eeZEiffcPMrzp2polkmtUCLSdXTZTtqO/BAR+wRhTMI2sKMAT8UYQkAkScsQX7blQHWl4UgNParSbdqsuSG+HxIDBgatKEJFEc1mTIxAEpLt7UYGAXHoY2o6pWIdREzo1anVGkQRCCS5zjS6ZVBcL2MIHUMXSKFj6AZCtf+4PLeB0DNce8+9qFKBVuAxd/wC0jJpBWC4TYxkmpbv4bs+TjpF0tFZWirT193JyZeepaIsJno7ePrwKeamT1FvljjxykEOnF+gN2Hz3MGDHD9ygIUzR+nqGWLj8AZ6e7p54acvMLRhiqRtMn12ms179/FrD76JchQzfXKabdu3Uml6+K6LkCYvP/cUt918F2dmL7Jl4wghIT0Dk8xOn+eJ7z+OYZioOEAqCVIhidsoUu3y6EZdHmMLgS4UKyWwczaVWsD6gounm5SrMZGKaLbaBohyQ7GyEBH39VKcreBHgkI9QumCW7eneHW9TLW0xmqpyY1Oi5tGbA4VAl730Wf5vS8d5XM/OouZMgjKTU6v1pjq7+amiV625DP0btnFTw+foDefb4+h6g00XaMr38+JY8+xd8tmjpw8geGk8ZXg/FqL+UpE3pA8ddEnbQtUpFj14eA6/+YO8d+1KIIijGLWPD50x4DkuekAPWXQn02xqHXjJBIsLC3wwkPf5D1/9AFu27uH2aV1lGZj2Qa2JhnozjE2PMIDGzTU2Vf58k/P8OjLB+hydNaqMa1yicGky/Y+QU4PeOj5GVaaip0jBvMz5zhwbJpq3acrJTh+sclzSwFRDCv1iNvu2k6cTjA3vUIpEqwE4ChFWUmKvqJ1ucNbjdpe0LWWIpYK39FxI4FpSkqewg0lK65CVzqpqe1sH0/y4xdOtLtNFRHFbUKaEO0O2LIdVCgQGigVEasIW9MJo4hQCDq8Bjl/hUokSFsCT3fYvHcnW3cP08pvIH7xeTKD/UT9fbz6zCFCYWEFLZ6/sMRjn30P3/3CQ+gzswxv38LgWBc9Q1kuXlhri7Jl+4UIo4CjZ5fIdtm8+N2XeOzpS3iezpb+PEYqxa137ySolFlR3Zx78ieUfBOlJHrUotgS9Pd2UCqUKVUKoJs0ay6elWQ0Z+DqOo1iiVA3kEJgWgmarsvWDovFmouhNPxWDQ8dRxdEUUxCxqwYGeJEFk+ziAIPJ2mSjAO0WEePfaphhIrbL7ZQBn7gEeGhogihAjSloYQgaEFoGLTKa8RBmyhYq1QRKOI4olhr8Lb3voPxiTEKK0XqjTqBFxEEPiqWGF1pXvfg+1gpB2Tz3WzuybHajClUl/jlX34v337kmwxnbRqxTbNawfNaWJZOOpNmdmaOvt5hrE6H3fluXpqd4dfuu5t6qDh46Djlhs/2LRvZsnkT4yM9bJicwDd1Xj16mPNLZ3j1zEVwYwa27MCOfM4vrHDHvis5u3oJM05w7uxxdl1xJWvlFqHbbMtrlMfgpi3MTZ9gYKCf0G8RS4sbbrueWmOVIy+/gmWI9kaQtkc5upxrIEUbSqUuN1JKCoKmS9Jw2HJXP0sXG7TqLtiKekOxVFHUQvCFwrQ02NLFxTNlclYbUVxugaPHVBNJXni1wgRV9hl1Pn+6yFs/dYpWw6UvKelLxqysV1mohkihcXZphes39DEVFpjctJO5iketXsUwk7TcFp7bJJeyKK0s0NHRwxV7dnPq1GkSjo2lw26nxsYewQ1jFq+uK9601eJzp0Ih/p0K4r9jUWwLvM8Vwg+vCftDPWaIDBVn9W6EbjE9P8cH3vlL/PHH/o7+ngESlqQW68RBSCLdgWlZnD99jC996k9wf/RVlgs+H/z2MxQqNVJWOwfxwdf0sX1DjvnVMtW6YqobvvXcDEYigawucuKSz+xqyHrJJd3Vy+7t4wxk2kuQt77hCj7zzRcwOru49YZJ+noT/HC6yHRTsRLCgq9Y82HdV8y6imYIZzyYbsSc9xRnmjHzAZz3FXMBnC7VKM7MMT65m937d/DSswewbZsgDjEsE0PTEUIgaW+d/TBAXo6rdD0fXUGMYrZUpVTyKbTgbE2xLC1+/u03EGZ6UGeOc8dff4j500e55ro9NLw6333qDG94w7V89S+Ae2jaAAAgAElEQVTeyLNf/go6Pbzt23+Ocekwl5ZKaF6aZG+CM9MlwsiH2CPwA3RT5/TZZbbs20m0eJLi/BwHXjnJzPnzQBfffeIQ0eRexsfylOZm8Vse6GlOHT1AfmCEVnGdtYaHjHx0QuJWg3K9yq27xnj0yCw6OrZlYhrtZUnK0Kh6LoKArqRDHLehRRqShmZgdHYTeg0iFUEACVsH10eoiISKWfEjHNHuaGxTEEUhsaYhwwjdsPBbLramoarLFGsBIoyIVEyt0cDzfWIBUtPx/IDOfDeuMNj3hl/kjQ++nanRBFfe+1b0aJY3/tqf8Pm/+O/ceMttJFM+qdwQZ8+dYHTT1fzn9/8Or7/vPs6cmEe3NcrFVcI4RrcNTN3g1PQsN95yB3PTr6CkwamZBfTY4Fs/eZbf/LX38MCbH2Bx+jSNKML3Naqry2weH+I1d7+eaq3F6soa99y8E5O23bRPhDx55AR37L8eJXWOnDzH3j1XUqrWqDRagMKtrDA0NEIz8LAdk9ToVhKmTaFcZuOIwdNffpQOJwumTtNVKBWSFiGKGF21SXle0J6TK9qz1/W1FtEy9Ozo5Z7f2sqxizHPn63SM5qia6QfP5nmfDXGSlt0TA0zc2mV7J7N3LAlxUdfLNLr+cw0BZuDEGN8nL85UGPECXAAy4COhOJ1+3QKFahHGouVFps7U9w40QvK5c733s/Tzx7HjdqAusj3CMOQzYO9vHTyFHunRlmYX6HSrNGMTR7YrrG+7vL4dMRNowZ/80ogYhXz73kEP6OT78geHB3s3Xt+vcp77rqNu/aN8MOXznK05GHaWQquT65zEL+xQrXa4t0/fz9vHV6n9crLfPKlef7oOy/QnbYI6y57N5rs25yj3BA8+vwK/+2de3j+8CpjPS1+eqLGu991P088/A2iWBFlN/GffvVuvv7dI5y6VOCGXWOgYHuPolapMDQywOJ6DTPZwexyhYRjEoWCfIdBFAvqzSb1lkd/vpu6B9lMjshv4AURyYRNsebTZYVUqmWsZAq30eDAWY+HHnsBw04SKUU+ZWAYDlLTMImJpUCpCE3TabbqGMJgqD/HA3duYrHapMOxyGeT+H6DbHcnl2bWkVFAJuvQN9RLY6VIV38Xa8tr9HQkadSbNGoe3cNDRNU1Li7VCIVB0KzTP9BLqVgllTRptFropo2pS+IITl4s8NjRMtraDAkFc26M1wgw00mCZovxrXvwZ46CY1OKNBr1FoPj49RWVii1WujE4PtoWkxNS/Hr+/L8y6EiGwYznJupkkoaEPvkEyaLqyViqbCEIlZaW6MYx0SGQZDNg9ug2fKxDJOkLUk0WyAkhXqJSqwjVXtz35HLsbxSAF0iidBkAhX5/MG7rqDT9Di70NYbHnr1BAlbZ8+WjbRaTVCSC8s1xka7iIRNqjNDomOIzROjVJamOVeAaO44O66+maW1dYLiDB1JnZmSIJXJoJsJqoU5sppLNdBJ2+18ynQyQYTiS987juiY5KarbqVuxXzj7/+e+WqFH/7w+/zJH3+I3ddcxejAMPXKGrFU2KbD0so6J198hi1X7OfQ4YMcefEEdz/wejrtFF94+BukYo1fet87qRUqPPHEY7zjXb/KmUtz+G6ApilM3WJo0xhnjh9j4or9OCmH4ZEpbEuwcWiVpz/2MZxUmjiGdCZFsRET+S7FUpFM0uLVsysYpsZYX450OsnMcoGEcNHMNLVGjGZezhRNZWi5PrFfIenYuKGGH8B8RTHQ28H3nz3Jf71vnLlKgw996ij9Vsx/3pniYi7PM4cv0WjBvdf38NOTdc6vNnntlQnq9ZAfHPdpxIJ8yuHJ993FxFgeuWsXv/2lw1xaqSGtJNJM4q3PYiTStGqr3DSep7+/l0888hTLhRLdCZMjp8+JKGwvk34WR/wsvlABt+2cVOcbEbvGB+mjzKX1GhuG8lgCFu1BCkGSWOpoKkCGId95352ceeY7yGKRnZ99nsG0xm07Uli5YV586SR37M0SyCRzyyXuv7aLdNcIH/zYYX73F4f4+lNr7JuAo2eLLJdMxjsCpt0sW4bS7L9mB5/86gtEoaK7O8VqyaXpR3RYOuVQ4bVchlIxWzf2UfclZ2YLrFVamJfFxQEGo/kMSilcP0T6FZx0J1HURIt8ursydPZMIO1uDr10DD8KMSRYtoFlOkgilDSIwzYPQsQxKgrRpUcqG+JFGuPDA5RLJU5cLJKxFR3pBCU3RotjUNCbT1KvuximTrGlSGuKQGhYpoWmwUA+Q19XF0dPTCMIOD1bIekYpNIObhBRqHnkkglqVZeslUZaNm6jStrScVs+JbcdkpuwEigV0qU8VgOF7wd0Z7L4cUwzaJGVGk7WYXzbGH61yty5VV57/Xa+8sxxbn/N1Tz9o2PYqk42kaBcL6JpCVToooTEBlyliMMYOTKBVynj+QGpjInV8rCDAJTgfL2OUgpLQf/mEebOLYPRjpVfbsW4MWwe7iTXCd3D/VQLJWrrJaRuEAYRi40YUxM0/QgQWKaOcus0AkHgesQqRtN1VOgRo+FYBqES5BwNISEUGrVKnXog2NCXpun5eLEBmqReqZLPOWgqZHbNpSvTSRR5GFHI6WKTz3/un3jqicd505t/kScffoiLF87hJLvRpSLb3UGmM0E61cuRgy+wZeMoDz/+A6QX0sTkthuu4dTxQyRSPVy3bz+f/fqXef/v/hanz61QKRaI4oC+yc2UygvExSXs8a04mRRjg8N0ZPPsvjri3Tc9iKkHXFhqkU05oGIavkRYDilTo+xGuG6Lar3FQD7NSiVg11gKR1OIyOPwrA+mha0bCE3Qcj1SifYGvxnB9iGHwU6LHx5aYaoT/EqLsxXBzf0awxv6KCwv00gkKHuCfSM1ymWdYysa10zGPHsqphKaXDNu8p2XS7x59xgfvXULJ2shEz/3Dvb86p9x8w3XUyiXyCYsku48HVGd0ytlao0mm0fHef7iGnEcIGs1Ts6vCAH8LOqi9rOotHtG+lRTSoQmubErZq0WEGiScsNjptigI27w1hv205eD84dP8+1Pvx9e+iGZ2irPr7n86Pwir7lqjIm8oN9cZHrdxgt0upM6qVyOa3ZOIvwaW6Z6OXS6hIx83CBG1TxOLEdc8hyySZsrrtxEV0eWu68eZdumPgIv4hdu38rNe3qwdMEH33sbuzak2TA2yGq5QUcuyT03bmcwE7NjywQ37t/MxqEOto8mWVuvkLAkN+0boV6rsX/fXgYnp9g2OYSlSeYWq7RaDUSs8OOo7TsOI2ytHfwaK4kSMTLyEUaK194+xYapCTaNDlCYOc/FS8uMdwt2DOssVTXuv+NKhgbzbJsaZPHCHGndI/B9JvI6zdDitlv30t+fZ8/ujRw/fp65+WXqLZ9ypczQ8ABX75wgMtNM9qaorheY7DcY6VI4BnhGH2GjRRAFOJJ2hx0DgY+QimbQxrmlbB0/8PECDysyGNzcy8DICE8/c4RKIaQWaXSqGvXY5MypBa65bRuaMFk4dRYjlSDyG6hYx5AaQgjiUOGqCC3RAZrCbzRJphOY1RaWaVJ0PbxIoamYkclxZqcX8SzYnDZImYKKr9rhwjmLHUNJHntumkEnwpIxgdtktdDiwVtGcZwEG3uTpBIm3cEq59YCNudimq2Q19+8ib0TneyY6KEzZVH2QBLRiE360xbjG8fozibpyiZYXqvxhq2CSl0xnJFM9DmMDw/SncuwYzTDcsXj2uvv5vipw/ztJz7D449/hz/8wB/z93/2p+i6iWmnSGY0LNMBBYaRoLC6wvjEGM+9+ApvfcMtvHDkPLqEmlRMnzxJtRVx1aYRpk+fYf81NzGzsAKhR7PuMtDTy/riAZzEOKVWga5UN6lcFj+M2XrFLqrHnmFk2w6u3TqIFyp2dDdp+bB10MZrtviFmyYwDMFVU9006h5/+NbtrFQickmNpuzgdVf1s3U4zb3XDrM0v8R7Xr8DKS1u3TtMfyqmvzPJcrFGNu0wW4k5teqxzYF9wyYvTZe4YU+e751u8sa9JnUvIJlKcvKSi9B1Xjgf8bbrHLJmnZ5cgu+fKfPuXcMMGSHJ4R5u2rab7/z0KL9841Z22A0ePXiUWt1DComKNaRfZbwjyfOLZXb0ZihVm0utIHrl/4pOUZeSu3eNqTO1kP965RAL6yWerytqfkhSQNFVCGng+zVSmsnn/+dvMjmUZ+mzn6QjbTHyseex9Jg/fsf1vPbqCY6+epTf+/gBPvL+e/ne89OcnV7je1/+Db71+DFqa7OcmS4yNWDgxZJnvn8KXYf+Dsl0mCchGxQ9m2Tc4nwxoNtWmCKm7sZ4uo4W6QhD0ZExMGWDhm/SlU3jhy3QM9imQUfOZH2tQEfSI9T6SetVzs5V6Ug75JyQly8JBBahnsKtNkBqqLi96LAvd4wq1gjjCF1rFwbLilFuHVMqkrbg0mqNjpQi50gMAxZqGpBGRAEjPRpHLlbYOqTQ9csZj5GJH6ZI2hJNxZS9JqN5AWHApbWQ2O6jw9awbEXoehy9WGAoBz2dMLsMTjpPIzVJY3YawhahkHghICKkalsRBaBJQRi1rZG2Y2NaJktrFdJ2+zoOMb0dHUx0ORw8t4a0FD2DAwyPD3Pm8AkaK6s4KQsRt0cIWqzh+S2c8S249Wrb+55IEFSLKD9iqeGCjOnq6GKhWCSRsNnS6WDhYivJ0ws17t7dj6Eiap7HYiVCRjXu2p7m6KUqHUmN712EXilpRqB0DeXWmeyG80Vo+GDYNq04xgR8pZCx4vZdQ3z34Cw9OZ3BfIalWkSXIzh1sch4ru2dX66DKy1Guy1CV1FCx69UGO1Oci7s4/p9e7j3Tffx+b/4CPvvuofGeoXG+hpKXf4NhSKZdiCWxLHP+PgUz73yIhu7Enzyq9/nxiv3cvt1+/irf/oCn/rVt/KBrz7Mr/zG7/Pic8/j+i6REmwYGee5Q0+y55rbiKSEsEZ2aJL9O69lYNziI7/9LtZOzWOGIZGIcMImMp9m0oo5t+axElhMDuU4dGENW4eyr+hKJFhZr5BOmAgE5SAip0nmaj4pXQPZXjQKYjQEui0J/Ji+tMAvBJQ9xd4Rk5mmYOOww9BYJ4/8dJWPvGMrLx5d4NGXVnnnXWN88ZlFrttgcfv+DZy4VOTLz1xgX3c/X3jdJhqeJPnX/8iXf+cD/Pm3n8bIZdGURJfgWFAMFZ1SZyoRY2U6eeTULENC8cz04s9k/Kf/e3/hRHdazTR8XjOU5WKpgi7h7hS4ERxsSGwnptxyyaUSRGHI1z7zGX7jxk10JQV/9uwF1mstHEPwmYd/Sq9WZv/uTcz4h/nWQ0/yiY+8g9/7zFEmrvpjJneM8+nfuoJXj14kECMMjDtM9dmM9rjUGjFdw91E1ji7hwy6cxZ1D5ZWW3R3pymWmpQDnY3dJkvlCN+ts2GsiyAwKNQbRFFE4DZYrsSMdTsY1hhRGOBHEqE8RsdDLEPScENGJhSVissXnlwlkzJoNvz2lSyKCZBIXyG0CE1KolgSy4iJvM077hlDailMA1pNj5nlIsWaYnJAZ60qqLoemUSCZqvFHdeMY5mwuN5kIJ9ACAOpacyvNkg4OlIFdKYMTl4s090nCWMY7jKo+DaOpfHA3Tuw7bYUJQgUxWpMnOjl779SoLEWIEWEjkCTGsFluRJCEtJGWcYoEobBUqlKIuEQBAFKaNhSsVQp0+XoaJYgDiJWLs5RWSswuWUKb7iXi6fOIWNIWA4qDEEzCEKFZkjiUBIEbcj3ctNtM8U1k8VCicGOJP0ZB82vE0tBJmXz4H0buXvvMGfPLZBK2vQaLTb0ZTCsNLcUCqg45gFMWpUigR/g1taxjDxKamixSzVySDsWli5Zr7ssVWMGczrH5mr8wb0jxEDB1+hLaZRbIXdvzTCe8yjWIpq+T8lPsFD26EpqtDyflXqeRsvl7Xu6iXIRYWWFjZu3UFmv0dvTTRSG+M0autDRpY6eTFBZLqGbcP70q2ScLJ15ia87XLd/B6dOHGd8sJPD80tYUkcKA92QhM0mURDTaLp0dnRhxCF62kZrWuQ78gRRg1pd55qpNE5asFKV5LMZdNlFFGsUm5Lr+n0GB/O4oWK9UuH2Kyeotnz6OxMoIam0fHQpiUJFFAaU6y5pO0ZKkzDycL0WPR02l4o6K6UqzUIFkYWLixCnYjRP48yyx2BPiet39vGuPz9INmXzO28YJ3JLnF9uMdZlsl6scOT0EmdWYbWywgefkrx75wDmD/6Kb3/tu/QOD7VZ1rGHZVv0G4LXZyUzQUxFCfxqheGEiRZHJAydVnhZlP4fuVO8Yapf7etLM+AYrLg+Zqx4pe6SNxOkdWhEUPU8+m2dJT/i924dY8J0yLYKZP7uFYRQXDVicWbRpdMx+Idf28avfPIkKRuuHEnyvrt6eGXJ4opBjUK1xacfPcP1Ozp4/pUCQ2lIJySHLsRY+RTfPRczmgVdC4kiQYqQOIoIQolpaDSFQbPVpBlpGFEElklCgzgMSaU06q0IPxZoYYSrIJVMoHyPnpRGEMXsGREslGJqLlyq9xPFHkJEJC2DWlOQ1GPSjt2WiEQxWDoiVES6REUFDFOiKUGAxLFNiAPCGJK6ROcyZCuKqbshaccgjCBlKGzbotIM0TRBw/Px0GgGIbaukbZMwhiKLRfP9zBNCz8IcBwbwgjpe6QcHdtOEMk0yxfnkbaD64dtKlwUEat2SrgfxagYTFMnVJIwCJAyBt1GRQGOrhPFiqGMjTQdzq+sYZkaCUJC0Q6UndgySWHhEsWlEknDwCUmTnZgpNIot47h+6w2Q6QKiSKfDjtBT9aESCBotuezUjE0OcBqcY1Xz62jd3ZiiYDVSkBXJkkQx4QtF1CUvBjL0EnbBlW/LQHSVUALHVtCRvMp+5KpDouaF9L0I6Qu20nrKkYTIHUdooBGqDA1AynbSfVBFCJp0xstHQxNQ9NE+zXyXcauuIW919/OpWNH6JscQ4YhWhjhhz6x20SzUsxfWiCSgqStk+/o4OLaMjJukZYZ/EqBczMLbOw2mQ7z3HnnbZw4fIZaZQ4Cl+7+ceYXTrJ9z7WcnTnP8IZJOnJphrZfy+DgIN/4q/dz6XtPsRKDYWksuRHS1mh64AURPY6gFSnKvqA3AW4k21bLqJ2K7rkh2ZSBiUCPfEJ1GQeSsDB1CVKyVArI2IIx3afYgJt7QDMUet8gR1ebvPnGHjAkZroPFTQpLC/zpw/N05vRmOozGeo0+dhTRW6fMFks+ZwswqG372DP9Tv4wkMv85OyIiUVy15M2jLpMjWO11wsLWJAs6kpxZitc6zU5MJamSOLFfEffqb4R9dt+JAPeF5AXijqUYQhdBa9CC0K0IUgEoq5Up2cpXHzZI6hKOSXH53m0FqdfEpn/waHt9+Q52JR8qPnLmEQ0tvVyZqV55s/mmFrX5Krt/QwtW0zpy+WGZ7s4Is/WmdLj4aVMNkyoCibA0xsGGLDWC/z6x5d3Z1EQYuB4SH03lECYdLd20fGCNi0cYy8E7Jv51YM3WDPsINlm2weHmJwsJtER46hZMSmTVN0JRwmN47jhZL1is6N+ybxtCyVeoTXCkg4Jjfum+KKyQ4wFIX1GkLXmBoSlGsNmp4GccDOLRP0Zw1GBrNsGu7iqn4D6XSiuT5dXT3Ulc5oTxYnaaMjueLKTbjKwrUyGKkELWGjWyZO0iBpxNyyfyOLLQ1VLNPRmcTIdjLQ30N/V5qJkR5u3dbNQjFkYkMfhZKHncmyIVOn5ZksVZtIpdqWOGEQysvJy1Lgx4qkrdP0QnRdousWYeQTC4GKYmzboNYK6Erq1D0fX0VEcVuXqQgpFipoVor+0TzVSgUzUqz7MY5pkpASr1Ygr0GHadCVTiCiGCE0jDhAtxwUJlKLKdTX2ZZX1N2Y9949RS0wkabDyGgfATqdCR0naTHQkWHLeBd2Zyf9PWm6Ox0SKZPdQ1lUOkWGgNHeLJ5pk0hnsAyT/GAnG/tyRMKk22mPDzQnCTEMjw2R70xw4+Y8mukwMDqAaRp0ZhNUaiHZzk468530GTGlapEwguGJSUQYMDg8Sravl2LFZ8st9zA5PML0qVdwUjmCMMatlwjtNLv6QmYKCq9WpKO3gwPHZ5naNgmepFBaQtfBtCVepUk5iMh3ddFqNRjdfgX53i4cGdAMHJJWkUeefQWVNNiytZ9cR4J0yqDfVPT1drB7UwdJDEIp2dWXIZFNMdafZSDrsGE4h9fyGBsbJJtLodsWffk0frNBZ1f3ZeCZwZUb+0gmDLJeHV0XEMFMCbZs6uHoUpMHd6fBkJw7N8sjzy7ylZfL3DCV5dxCg2bN52Jd46YRjbs2myg7xfmVFr+0McHA9j6Kxxc51vQ5Wfa4qddmqRqy6nkkTZMBUydJQEbTKHkBI50JklJwZLn64f/wRfHQYu1DvzCe4a6xLJ6v2shIFbHRgTfkbd7cY3K82mJrNslgUjIh4OBsmQ8fmMeQMNVtcOcV/aiwQk82SS4pcByTc5dK5GkyONTJN567yFd/dJFXDp/E8wXXbDZ5/mCZu6/N0593sBzJ4qUyN1yR5bmHjvK2B7ZTXV1jz8Y+Dhya4Z2vnWLPoOSGXRkunZzl3Mkl7rxllNbiJTqyFtdet4VrNya5dmuSi3MVGvWIzX026cYCo5mQK/ePcc1mhzt22hw9dJGt/Yr5MqzXAjwUd+5OMpgRnLlYYLWqMDWBHrmMJ1okkgbFps7qapndmzsonTjJ5FCWF48VOHB8hj945052DgruvKKDHrWEXp5l+1iKtaU1nnvhHA++ppecFrB/CFJ+ge8/s8A9V+V56tlpEo0Cv/ngXobyOlM5hV2e4bGnZrhueweXTl6gJx1z3Y4urp1KMNUnWb4wT1Cv0sKiqQwiFLqhkZASoWm4QYBtGASxwJAasRDEKkSTOrlMB5FXh1gRKtFO27ZtvFYTITQi2h5vzw9wXR8hbJIdefSECV6AFBq2Y+DWPTTTROiSwPOQmoYlFC2hYylJZ1axcc8wp+Y93njnJn58rEilWueNtwyRd6BVKqJXFjg1Xebndmk88vwqVw7HXNEbkxYKM/Z4+uASS0sVBiyPi/MNNg9oXLttgMGsYqrP4F++d4Ehq8T0XJVS0eXGXX1cP5Vhz4jDcEbjuRemmRzv4/Fnp1m8sMI1GzNkohYbey3SqsUXnlvgzp2dHDlb4NBLJ9jT1yI3to2fHDjIX/73j/LUs8/y8Je/SENavOae+yguXsAPQrxWwOSmUU6evUQXirKn8OotXjxxhntuuZNStQ6Bi7QMwmYdU+qU4hzDg92srq+S6+khn83gNddpNlqIRCe5C9/jxp15slqDVq1JJm6yXgrYONbJc68W+E+vSbE6X+KqTRZXT9r84/fneese+NyP12m1Iu7ZmWHYKrF9yGal2OCleZ/7d5jMF3y6UwZrc/Nszgg29SqOLoTcdbVOdSVGS0tqXszCapl/emKJx16tk0saDOeSzCwWEEJjuEOyq0fjumGfrXmbRw9V6E0JXp4PeUtS4nkBnpBsSBhMaAH/ZXuOdBDjiJAwUFiawW29SUY6Dc4u1PjHQ7MCIf7jL1r+VZTTlzLVn17RzzXbephdczle9Kj5Hsk4RheSJjFaQucN+ST/49UiXzq+iFLwOzfluHLrOMMdMWvra6wVCoz0d7PWSvHqrMv8coWuVESt6tKzZSe9ep3X3mTyhS+eZ/MGm/NLPmEccetrNvPEY+dIdSXozmc5c2aJrx+s856rE1xca2fxFUpNTAFeJPmFX7wGf/plZtw+jry6Qld3hs6uBAkd+kd6SEuXnb0VTq/Y/PTVAoZhUAgEW3I+V46ZPPKq4OnzGpVmi209kvFeCz+MeOFCTMYQZGzFPdtNluoxZ4oWS+sRQRSwa9hha09A0oLZxTpn112k20ST4AubzrTE9WOSto5u2cys+QgREvk+fiiY6lPUQoe0GVNsSqrVGpadoNl0Geqx6OlIU675uEKnWmsikNTdgJ60RtJpM2lagWK6aFFpxeia0RYKhyEJ06QVRCC0y6ELOkKApkkcKZGGQOkmbrVE0kmRljFzdbcd+KDi9lVbaERRGz3rmCbpbA5b1yivrNOZTdMKvHYosPJJ/L/kvWeQXNW5/vtba8fOPTlrNBploYAAgUhCZBMMiGhMNNEYzDE2BhOMMQ7ghDHBYKIDwmAyGCQbk4MAESUkoaxJGk3qns690/p/2GPu/XBv1f1yDr51dtXUVE3o3r3Xep/1xufRBH4gcESoumdRwtdzLN5zFictaeKXd71Oa3stfrnI1kGPTSM+5x7dQu9AlfHhEarSoiEWaoWXlWDdjjKJuEZjykIzLCwCdOkxXhUUHR/b1BDCxNRcyuUqAkXENsgUFblyqAOaMBSWqYEPtQmNbEUwlClREzcpVx10qdFWFyVbqCAEVNyAnoxLwgIrYpKM2NRakkzBIV+okOrek+tv/ikrlj9AfUMd8ajks48305K2GBgaI+oUeeDV1fzs6h+wYdMOnHwGTfepFLNI5WG2ziURlHFsQW1NI/V1cZK1aYSZoH5yC7894wRS9Wm27yyQiARcvJuiuT3Bs+sErqbY0O/SWBfhrx+McdgUyeCwYunsKO8O+WT7q4y7immTLJJVB0eHqmnwyVaHRy5u5c1NOTasL3HkXkkGM2U+3OYwp0XQbsG2gkVNcxo9Eee2JzYxvzvCaNEjYukE+Bw0WUOVKmiaoC4Knh5jXV+F7UWft/sUH548hy6typ+Gfby8y8KkyepClfqISVXZdDZYTDLhyrd6eGzj6H9bf/V/a/P2vy9LEyyoi6pbj+pmn2l1bPlkJ6t6i2zQNb6rF9m+2xQW3v5umH+aKMhYUYsHL92TPWY3snWowuD2z9m0tRdDh/pUjNraNKt2xDnvK0088FY/VErUu+PohiBT9Ng55lHMuXNYwHgAACAASURBVFx6chOPvRWlMr6Fxz6APadHWDdQ5uR5ERbNTaJ8nTU9LjllESv1MXtONz1bt7PH4oX88q9bsaVD/64CuibQo1ECO8LwuEN+vAiBT33KxjA1dN3AllG2jCjG82VSSY/ptYpcVbBmQNIU15FAjZUjloxTqZQYytmYRpQoFcxkgs+376A+Ab4vSMZ0dN2gXK4SEOZ1atNx7EgCJ6gifEEsGgKW7ymcapmdY0WUAB2fkiNx/JCd3PEUbU0pCvkqGgG6NKh6BoY0qGlIks8O4wQ+rhuQLQSUKwrNNCbGs33Ax/NNlCjjB4QKhFoSO2lSG42wa3iIwPeQQUBzOkmx4pAtV8Lma01h6EaoMet6VJ0AaULMjCGEpJDdxaG7z+CzDdsQdgKpBIYWUC27HH90J6cvm8uyy5/nB8fX8PiKLfSWdHIu9GV9pnekmdtuYbl5Vq4tsTUPNaZi2qRaanSXtf1FdhYEMVORtCRoBn7VoSER4ClJ1hHYukTqOq7ro0kN29SQuh5KpxqSfK5AvqSo+sHE2JyGbfiUHBAqwA8USoS0XHET0okIkYhFxQswNcnYeBlN0xjLlUEE5EoKXJ9zzr2UyR0Rcpk8ejTC9t5tpAMgP0bfzhHW7Mxy8XkXsH7DZgK/SKWYQ6lqOBCQaqOYH8WM+9QnmrA1hzn7LWYsU2DmXgdyyVFL6c35dNVaNNdEiZiKfLZEpuiiPHBEQH1EQ2o+J3cK/jGgmJFWfDYmmFJrsG+nxQPvVrnhsIDvveAyvcXgk36PBgmL6iOcdWyMVz8Y5R+bYVpjQKcOdkxy3weK+24+mCtufpOLj5vBYN92cr7DeKZKXEBnjYWtexy63zxymXF+unKU+9/PkpaCBXWw3Yvx+eWz2LxqJx+5Bl0ll0UHt1K0TZ5ePcz3VmxluOIKX/13dSf+D4Li//0jdKZtddS8ek6d0kDcdVmAy3Grc7ywaQipSZoswXFTFJ5h8Pf1FfaYHOXXF+6LTYUyOjEryo6+HdQkolgdnXzt5+9w3bcX8dDNz3L5GVNYt3mUDzeXWP5RGQ+D+TUO93x3DpfdP8CsWpcXN3n0jTl0pQV7NAm25jQGypJcycWQgquPb+fBl4eI2rB1xCdXdjEneP8IAoLAByERQjIhdIYQoFDETYO6WDPgU60WyDgVqp5LRDdojsRJRzR2FioMFvOE6rwKXYPGdJKiE5AvFVGE7OBSCjRNw/W8iSc4MesufJQnQm6jIFQrFJoOCLQJ7RcQIBRS0yfoonyCQJE2beKWTkKT2LqO4ysM0yAZizKWK1ByqqFyG+FraJoe6stIGRq/CiYKMAGmGaemvgGvWqaYzyD8AIQiadvoUjBcKCGUj2booHykZuC71VB6wfOoahqW0LA0jURQYkZtjA8Gy9REIxPC7BkWT4MPtjqcd8JUrvjzdsbKhLIFCDRdonyfmK2TLwdILSQ4CAh/Lybm8TUhqNcFhiYoeSGhsKvC8TZNKALFxHMKEAT4Sn2xNmqCpFXX9Yk1ntAsmVgfNcE6w8Q3X4WKk4pgYsMLpAZB4IesNQqEDBUdD1i4J+eecxrjYyNotk1fzzYqQyOkqPDWp5swUw3sv/cB7Brox1dFyoUcSjlEomkKvo0XOEQSNomIxDYEU+fMRI810jBpGleecy5OsUrRdcg7HkoEaEKGNG0SfD9ACUWrHbCzLNinRdGXV2zMKOIGpGzB1KiPHRGs2BbqEp2wm0UhF3DmkhQfrhvmzg/CusDMtMbRjQEnTINgUpyH1qdp62xgrGc7PzpmCmvX7UAJg/qIhhGJELMUz74/yvUrx5jbIkiYYCP414jNeMFn5Qnt7B8z6I3avFb1WL5qgFUbMyLnBv8viPL/U1D8f4ZIQW0sduG5u7fc8+u3tod8f4GHEJJoNM45Cy1m1fj84Y0xOmd2s/8kk68cOJ2mujiNba28veoz9rvyBS766mw+fGs7V39jKite/ZysnuaZ90fwlUAK8JRGq+my91SDpzdo2IZEqVCvJACCIEAP/AlWbKhUfExrIkQUofGoCbYbKSWalCilEUzoHmt6aCRChMbYGklS8ELwtA1FplzGUwrbTBDHoaIUedcJ+f1kaH5CgOd5KBmKk4NACklAODAsNYnyQ49E03SUF3zBfi6kwND1iXEnie97KCG/CHGDQKGCAN0w8HwfgUATECiF0CQW0CwD0AyqKjTYqueE6oJCQyiBFwR4KgA0pC5QQiAVaFIjUC5SSnxfYRoaBhqpiMFIPg8yLMRIGRJkeIGPK3S8QLFng2Kk4pPSAipGgv27I0Qa61n12gawbCL+MNscm7gJ24uC3lEXQzfQTfMLpheUCLkpfQ9N15EyPKiUAlOGLEVCfaFvjhOETeqBYIIgISwp69KY4LgMJl5bhOAWhFT8Sqkv6LekFiovKjWxHlJHBcHE4cXEZw33ldQkBAI3COnLhJSEI7rhrrrpR9fjV/KUykWiOmxZt50O2+ehl97iqEOPwrbSlAtDVCo5iuNj6PEowtMhkqRSKpFqbSRmFIgnm+iaNp3azm50Xedn//VdBvqHGAm88NAUEt93QYUALRDomoanFEIIyhUHIUEX4UEQBApfhWsmVYAiIAgU0xtMfNdjS1YhxL9H7ATHzBScVK8Yj8ToHXTYIpM89ckIlx89jd8ua6GSK/Px5iHW95Z4YlOVDzbmOHWWzuKZNt98pkjGDbspGpJJfN/lwqkJfvRu7/8wLv039yn+fxEzqEkmD03UpO95+PMstakEY+OZCXZHgaY87ns3IGVWuHivCEYwwO+frXD74+vZbUYD1938NU67fgXTo7BtXS8XnLM7TbFxymaMv701jKaFQt6aaWBJjYJu8UqvQWNaCw1HajjVMrpuovAmJALAEDoiGRqAQqJJDcM0cV0XqUm80Ekk8EPmaiaMRUxsPIKAcRHg4SCkIF/yaIpF2VGs4AdVcoRAa1sWSoWvr+sKIfUvvAldN8KTHBBCm/BIA7xAwQRxKKYCGYJSqHjtI//N7C3CAycEQIWSEt91kRJM28J1PQzD+EJVMCCg6CkSMqDOtsmVq4BG1asQKED5IXAoQVWEFWlflyjPI5igplIqJNj1fB8R+FRVQG3EYKQSqvf5nsI0NQJdZ9xxKJSq7LcoQiYb8i+6LXWccojOzDmNXLd1A4MVwegQnL5/HVf+bRDTjmBHTUzDRE5MxkgJOuFYoKdMpNAADaUUQqoJ8l5J4AuEJggChS41hOshRPg3KtBAgNQEmq7xBYO/UOha+OylruH7Abqm4XruhLcnw/UKQrBAKfQgBFYCMKxwDV3HRbd1RFUiLBk6+0G4Z9yqywdvv8n0OTMpVD1K+QxCVdmc8SlWXNINzezq68GtFkN5DDOCHolQyVWwDJt8eZSWWC2WnaKuuZVYqh4RSDRdYFkxPNMkImyU8sI9YxgTAA+B56LrOpoMn5dtWROELuB5YVQRBOH/uW4V3w8P2C1jHn4QHnJMeNMAz29QfJIQXNJSYOEci629LgkBf16xieHBEU6dGeGsR3ZSDRR71Equ3UvwbD/c9nh54oAAS+hohkk6GeMvvSW+7Ev/0t5YSlxN0lDbTDaf57Slc1gytYtvPvQvmtJRcnmPn76hOHyGycUHBGwa9Fi+dpiXvvo7DuzQaEtFqW81WffhBm7/fJw1AwrT1BFCohkaUc1GMySmaSEkE0AQAmPMThP4DprU8d0AdBNNagjl4RMQCB3dMPF9hTRMfN8napn4wYRc6QRI+b6PQCJ1EebfBJhmDOVUqbcijHqKSWlJIZAYuoH0fSxf4egCPRIJvRUpMWQolRB6pSFDt9JC/l/H88B1kSL0jKSuEXgT4K0UMtDAMPE8J1QMlAJNyi88Jc2SBG5oxIZhomsCodkEvhfqdiQsxiplSo6D8F28wEdgAD4KjSBw0YQgqskw9+aFwCulAiVRKDQxwehsmJQDD1uLIfHC0FlKPF+hC596KYklba5/tcojZzTSP+TyUs8gfX0pTLGNnTmffFDhtGNncNUj24jGYpimiaYboWyDJtEAoUmCwEBO3KnUJUqJELRU6PVJIVCaQpMTlXDfR7fNsFXI9xC6INCN0EvSTADcwEVDA11DlxKBIBABge9jWjau6yOEQDMtfM8jFrFxqg6W1Ah8l38H3xqgGy4IEXqXQkwkQcLwOrAU2/sGmNzdwfjIGIauodlR3PwwDfVNKGWGRS1dkBv3iUQsnIqHaSewFSjPR9MlgZBEogkEUC0Mg4qjmwqkhqZrSGFg6KGAlSYECA0VeBMHi0QTYgIIPaTU8H0vHPn0fcpOOHrp+T7Vapkg8NE0+UW6QAkBE8zpvQXBL3ok19R7TIt5zD+qgRVvjbKur8hxH2T4zhzBKXPgxR2C+3ek2TRS4piFXazaPkomM0Z752TGMzuR0kJJ8b8TFKOm/KeOIJGqJRGJMamzm3f7Kry8cT0L5synmBtFAanA58XPhnn5c41z9onzlzMFy98q8PGugJmtAdmxIp8MS9bsFJhm2F0kNY2IEcWyTaLRCKZhIISiWnHRjLB1JJ8bD0eWfJ9IMo5umjhVj7Ij8L0qQkjiUsfDx5E6XlAM6Zc0PTQ4XcM0TRQBpXIVyzAIEOhaCEa+61OUktqqg/R8fEsDGaDbNpbvkbZsXF1HhT9G/Nvb03Q8P/SYfcLJFOVUkLKKGY2gPPWFh6dLcANF4IendjSSwDQlCkGlUkYIjcB3MDSNarWKL4yJMC4M/ww7QsQyEULiSgNfgBweJKpLLEsjVwQ38CeEuBS+604Ympzwwjz+nfPWpQ4ioOqEpK+VSpWkbjHqFWEih2XqJgqHWCAwkxan/HEnd5/UiihlWD8YJxJx2NDvsNeiJq55fICKrCUVt9E0GaYNhJhQQ/RDwJtIKSghMHQtTIkEasLoJVLqKM9BamFDuucHeGUHywgIPIUUHjFd4shQv9Z3HTRphGGvr/ADD13TiVoGSlj4foClmyEnoQJDGihPoUmDQPkYuoFmaCgl8D0PLwgQukQTMtwbhjEBnBA4FXaN5ZC6hWUnQt3qfC+aEcEydHzlgnKpejqm5lCtVDEMDbQqrp6kUqmEea/qKEO9VTS/Gc00CERAKt1AEOzE0G3QJD4arlMlEYngBy5SM9E1DTUR4kdsG4XAdRw0LIRSIeDHE/ieG4qwxRKUK2V8z6VaDeU1QpkDHwXoEsZduPIdxd3HVtixyyMVC+33L4dLhBNwzVtxVu0KmN1is9vsTrYHOukmCy0Wp6mlHanKJCMGypH/O0FxNJsXxYALF0yfc4+ZakBGaiiVxqmrixONRKhJpcJQI/BgdAQnUNzzZo58TnDtKc1c+eg4Zx3Vwmm3bqUnp2OZoRHEojHqoxLbtBkvlKlks3iRCPpESN1S30Y5N84PrzoDYfo0d7aTHclx1Q/uoaWlmeMOWEBNXYTaZIopU7og28OdT6xizWe9YUim5IQYkI7j+xhSEo/FESogVdvI5o0bkEJiSJ9Uwsao0cmO5Bkb90FJ4vEkNZ1tlPN5dMOgu2sq/T1bGRkaImInCfDIFooIKWnpqKehtpn+3j6UrmNqJoERhl52LIpSBsWxEb5y0BxOOXkpv/j1o3z8WR81NTXU1tTjOGUqeQ/TilLMVWmqCWhqqmXD1mGqnk8mXyYrQ8+6vWsSTiGDMm0iMsBzHBw3wPE8dKFYsPdUFu09B0O6/PFPr5EdLmLa5kS+TRB4AUJXCCkRmk7ZV3S1J5g/bSqpVIT+nVl2DmQYHy/juj5BqczUZIxLnx1mllVB2zXCeiOFj8fd/+ilpraWaCykXotGomGeD4mPwA8UuUIZt1rF1CUNTXU4bhXlehimHWoXux6gMZYrISmTsCQLZjZx+vHzmTqnmx29/dz8m+fZvnGEtnaboUoVS9eQygdfofQIslKknM+S9XzQLfx/5+KCgJq6RsxI+Lll4GPokQlJ1wAhQ60e247iIxDCRQmJUgoNgRd4SM2gWPUxhIZpaEilsOJximNZHCdMC1Q8ge9V0A1BxLCpekxU/01cpTDNOLlMP42N9Ujhg4himFGkhJQmKZsRRjMFOltt6uvq+HBNH+lklIgVrpuPwjKjDI9m0IXH/BntROJRPvh4E+VKBcfx8HxFTVM9ulLY0TheEMo4SEPDMG3cUolMZphSuYAMApBw6fOKXy11OWSWZPV6n2pVcP4bNodPraOl2aVm6jwSiQRCalQ9SX1NHYlUkl0xxer3PqSh1vrfCYpJW44dOK29ZnJ3HQNuiu19g7R3zWKwbwczps0gV8ySzmeJGBabtm0N1cikYPmn4DLCbu02F925hf6iSTQaFhp03SA3Ps7hM6dz2JJ5HH3GuaSmTMct9hLoDcTjSS45/5s8tvw9lj+X4oUVDxCMb2P5X19h11AGt5gnu9d0fnz1tUDAzy+9lr/86RnSdXEcsxZNBRiaYHAww9Rp7URi9ezq34GtSTTDwC2O8trKP5CvFpg1Yy7RVAqJT9UZZ3SkH4IqPTuynH7OLbQ2N1DJ5/jww3f5+fmHcMDXT2Dnzj5yhSJ1DfW0tNSxZeM2jjruWmqbOonaFp7noUlBXWMjlWqFU4/bk3POWUZjYxSwOfSIJWzePMCDDzzGk8+8Tt8ul/nzpmNJj5XPXM54dpxJre3EG6BUqlDeNUIm61LV4vzmjn+w8s0yMQGTJ7cwbVYzbVNaOOyrB9PW1kAiFQOGCEYrXPbdU1i58kPuuP1p3n9vE/XJZFivVmGVY0xZ3H3b6Rx/5GwME/5w7/OMjI/ys+sOZMnS3Tj3gr/w2qtb8QOfxS0pVu8SPLKxypFmiY+zBnV1aWwrgqkb2LYFmkRNhJ/RRB2zOy1+/qNlBJrHO29v56obn8a2TAw7hlI+QgpsPcmZJ8/huK/MYmxwgOaWFI6Mk7CqxNL17LXvVI75yr6s+XyAH1z9AP0f7qKuOUXVD6hp6uDggzrp7Iizx54LaWisRTd9dFUkV2lGBRXWrO3h8h8sZ1J7I6NjQ3iuQ0NrB8ODOzA1n4bGGPFkipnd9bzy3hCFfB63lAtbpswIUyfXctCiFlKRCnZEJxGPEfgGzqQUZk2UaI1OYrwGz3MxDA/p+dQ3x+nqqGXjZhfX8ZBSYUdi9O8awlWKI46Yy47BHJY0ccbHmbH7TC678BssPXQ/ysUCg0MFnnr2LR7843P07xpnxuyplAt57rvraupq6mlqn8yUKTOpVItUSjsZG81SKJS578EnefGVjxBOHl1Z6LpE6iblagU7FqczWUsuN0Z2bBeO4+B6Lt97JWBxo8+iNsVFr0NLKsLUA76K6N2KmW4mXd+IaSZpbWnGMC2qhSFSqov9ZjZwy4PP/U8UmP+Tqs//l1rWfecfrKKTplPb1E4BndquPakYMbZuGWbPGd3cec+ttNfV8as/3EsQVHGrRQxdw/UCTlqY4PEPK0SjGlKGYbNpmuiajijksVyXWt/j2FMP5ceP/IGglAPhM23qkcQiSYYH+9jcuwLT0Lny2zfz6NPv0trSzJ57dXLPQ39g5RNPcebpV1Df3kYyrnPd9Wey1/xOpBljoK+HOQvncfIpv2FktEC5kMP1PDI7e1ikwRFNCdYXK6Hmsakx55i9OOmmK4AIoLP+sx1c8K3fEg0c3HKOQzZ/ii4NDnjwJvb76kHglUEGnHrCVbz2Xj91DY0TLTgapqVTKJY59dg5/OSWW0D1gHD55eV3cvSyA5i9ZCng4/sVXl/5d067cDnXHzsVd/Va3tgyhKMUTXGTs85YyrTTl9I6pQlUHOJJfnf78zz4u5XM7a5j4PO1JBCYrk85FsXDp742yumXncSRJx4IQoPA44ln3uK6S++n5EwUHYRgyeG785d7TmVkpMDaTcOcu+xmFpo6bxervLzy2xx76v14vkFzzKTG0tiYKbGtUAr7PmvSROwovjBJJm0cN8A2TQQKhKSrPcazK28gMzTMt0/5CYvndvHjP75DTUfXRNuQj2ck+dm3FzO2ZSvL//RPSrvyYQ8gkAQaBNQu2Y2bnr0KTUXQ7Sj/ePF1rv/eIwgjQs/ATm7cdxKt+y7kvn99RNTxOPeso9jzhCNJN9r8u6Ly6SfbOfXcu5EKnFIOv5jj5iuP4YSLT8GwJOABMZ54+BGu+9UnVHP9SMPAKZYQapwdm29jtHeQZLwGoyHNUE+OaDpOvLaF/p4Kpx73fUxT59Zfn0ZnVyO6kLhOjndXDfPjGx9n8dIlmPYwN/zwG2TG8yw9/DImNbWx+7z5nHz8bnTNaKNarvDxx9uIx0zmzJtJvKaVXMFn05YdXHLFXQx8/gGtysHyoFBy6O5q5OhTlrH4+EOYtdfejOdL1KaaOPvsy/hw7Q6cSiGc/fYcgkCFhcdAoBs6xVwWt1wgqBSpej67NfhsHxek0i2YTTO593c38s9XV3PYkoUM5hyklSIIdD5e/RFqfAMjm9dw0VF7Mf+8G7/0pOKXdgOHzmpW11z+dTJunE39I5RyBTb1DNFUU0t713S6Jnewbes2Xn9vNSvfXo1bHkeKIGy1CQQRTQMjzDeFXxqWFUUKHdM0SMVMBtd/xsN//BEHnbwMhM9lF/6IN1//lPH8CJ9vfYbx4WGefuw5br31n5Cq4TuXHcXpZ55Fc9MCOjpn4qkAo5pj445+erY+RrVUREpF27Q5vP7GZn7y44cZ29WHpksiTpFyOce0RXNZ/ugdZLZsBJXhazPOZuGBM7j+73eGutBawOvvDPHj79/JyMggLekke+zaQdvcLr7599sY2bKRmvY25s85i7KsJxK1w4KJYSCCAB/BnMkGjzz1AJBlV7/LoXMPpatU5fJHf80hxx0ElNiyYYjZC8+mOxYQranh3Y13o+X6yQ73s/yXr3DvPa9zz5PXsOjo/XFy43iRJFdf8kc+++AzhkfGWDCrg7q4JJ20sYUk//4nPLVxlPO+expX/Opigtw4UlN88P4qlh13N0JYeKbNW69eTtoUmBHFwsU34ZZ0GqI6ajzHoXPq+OPHWaKWTlvEpN9V5KVB2XWwDYmpdNoaLXbffRIv/Gsr6do0QbUU5vuSjRy8dz233vsdvn3GjYy9uoGEpfFuzTTy+SwE4BFQKYxDdhfxWIRAtzlhyWzmLZrE9pE8XqHKpFQNK371N9rmdfDL13+LXimBp/h063ZOPOwXNHW2MW/XFg756lLWWhEYGmCvZJJH7v8Hl79xJ4v2nYVfKqAJn57hAl858QEqpVGacGjbuIHDbjqP8y87gkqximFFMGpslp38e9as7UcoB98PGB8eZMPaX+CNDdI4qZkbr36SZ+9ZQQqILpjCg0/8kkVLLsdA4eDy+Qe3MbxrCM+vkEzGuOuu98hkEvz2gTvZ/PnztLTWMKvzKAoyztQOm5XP3ULZ06kUi8ycdwaOr7jv7mv5xgXnhCG5aXHSVy9g1ZoByvkRNq1/Fdcpk9u+hQuWfB3Dh2U3X8VpF5wLQlBTM4nFe+7LrnEHpEQXEy1ZgYdmxiiVcpRLRUS1iOuEjD4FP6CrawapWIpTTzyBs84+hbFywB2/e5DNmzdSDqrEomGP6MHT65jX1cBfn1nBn15a/aWDovxykFjw8oZBYVUqYMKU9g6SjXU0NTTx3Ovv8Mjzf+fZZ59l0/rNHL73Aia3t1KXrkP5/gRrCiAlUsowMS5UWE2bUHsLvCKVUoVqvJbHnn2dML6THHn0gTiOTzxqo5txrFQry772dfxyjtGRDLvvsRfr1nxOvqIwLBMNRRCp51sXnkRtfT2t3buxc0QgpM1++y8kcIaJRa0JYfeAdDzJ2/9aRbGcx6pxee3NVbwjJK+8/jlvP/MW0k6AkaStRWfLwEY038Mt5HimonhgWz9BeQwzFkczbObPn4lhmBhSw7AMbMNEs2yEphGzI188y1y2SDlaT3T+7ryy/GnAACwGenuIRWLYXXvw0ZZB1q0ZpFRwSDZNZcXmYdzmDs498zf4OhhRm2isA9/w2Towwlg5YG0mw88fv48fPvwTLrrjBu4acqmdPInnbv0r9/72CWSyjmoxx8K9d2Nmdy26Jlg4r5HO7kY0N0N+dJTXV17NMScuYOtIAVXbwJOfZYmakknpBAMeONEYuqXT3txMQzzFlZfvxxsf3cgtd1zEqg9/Tnubjl8sM6OlEeGNst9B8wCbTzf2s6N7Gq9GW3EdjyndszEtk0Q0Rl1dC8mOaXh1U7BT9ax47HU663V++F8H8uPbL+P2p99mVdsUtmwd4bqjrkWmu5DxOAt2n8vj/7wVvxzwvFbP8l0ZbrzjJm587KeM7r8HVlMtTyy5lH88tRLNlBCNsH3LGPldnyGUYBc6H3fMZO8j5lIa7sMXFQQF3OExzvv6HuSrBp7jEbUjlCs+xUKZ5t1259KLH+Dxh/4BU6cy3jaJobU97Dn7VHxHEW+eQnY0Q9WXNE2aTkvHHEplwRVXHU39FBNIsG3TJpRyKPoeyUSSLVuHQEiUW+S1195HGhG6J3dz449+jxCC7MgYmaEh3n13DfFIjHLBYevmz8kXi2SV4h0RYa1usnFbL3rEIl3TzndPPIHExk0YmiDwPNwg1DoXSscrlxAK9MAn8KqoQFLxPVoaGmjqmEJz92wW7T6TO376E771rct55oXHeO/DVfiuQ3NtLfW1CfzAZP+Tv8ajr30svkQ/7csFRUU4TXDr/U/QYTqMDfVheoJPP/uMnYNDeIUsA0NDjI/nyI47zGxtoq6mlmi8Ds8H29SIGDq2YWIZBslIlLhtErV1LMPA0Awcp0LUNvjzc6+Tr/j4SmefA49gtFxh1pyZCCOBHUtR295KY1OSSZGA7ul78OQTz9DWMQmUJBAGVcfn9NMPx0y08O77O9j7wHMZ6CthGAFXX3MBkWiSqG1Tm6qhJhGlqSZFLJbAStQzed7eoAIu/u0FHHLK3oxt/5R7b3+IubNPIeoKDAFV10U3DAYGOvrufwAAIABJREFU83huhUIhSzU7hAhcKuVK2EOnCTSnTJPK0+5l6d05GrLNAqVSBuW69JSq9Dr+RNjmUyy5WEIhNahvbufJR14kWtdMMFFFVTUxnnjpGmR2iJFMmbNPu4xnHn0JTQosQ9G7aYCx4V4ELoO9W4l7khFHo6GtgfuuuJ0P3v0EKxFHyCT/dd0FZMdyrF+1gavOup2d/SPUNybAy/Hbnx3HmvU/Y79jFuAYMSKRODsLFbx4Ak0KNGBkOMvNvzqJS689h78/9TmJ9Gns3NHHcytvZcmRM1nf08eqDx7hpK8fBgQ8+cSveehHp/Prby7lwpPm0LP1c5LxFHYkBpqOZkRCbkBpYje0EmwaANNkZPsGBnoyGFJnfbyeTW+v42fHfhslXZQTsPuiNubv2U5S0xgezADjgMlr/3iPvq45TD3mIPY7YH+UHuEnP/4jhx5+OZoeIV+skMuUMWzFbl0JitJkj+mXsrW3DCrg6K/M48xls0g1T8b1AyZNaqOmsQNkDRVH55WeN1m19hne2Pwiy9c9z0N/uomTjtwb4VbJlxVFT2fVa+8TqUuRjsXwqgHXXnsqxx67mGefewVNawhdDV1imFHiiTSaHqW9tQVfWejROFK3UYGDGY2Qqq0nGo1hmjamaTE6kscydcoll8mTm3ll7bvcevevCQKfuy/7L4KVr2B1deP6YBo6ugzZ5T1NISwjHGQwDNBMPAFWvIHps/egra6OOa1RKuUqqz5Zy8bt29jVN0B2tI9NGz7F1BSV8TFmt5gcd8jRVF3vy00mfpmg+O/r8TWDoinlM6M5glcsoiuPFdefyaGz2rnwhINYOL+LYqHM6UcfQGdcY0prMwqJ5wf4vhcSo2oGuhVBNyxMXScWNTF1DSEVpgblqsvGdWvQdJtEMo1tKqbN7gZqKedyBL5k8SEHsNcR+5JMxFj+8DN0tHfR1dnOnJmtTJmUYN7eCwH4dPVqzj/7ZHq2fARujsOPPoJU2kRKg1ylSrZQRtgWoAh8hW0apFtqOfn8s8j399G/Yyffvfp+uidNRY8mUEKSLVWQUqexLomMNVBf14YQoTA4ipDfsOKi4bFwRjtzd++iVMhPgJ/Cd1wqroPrOGze3guqCqpKR2cbXuBTLhSwTBunosBKo0fiVJXk3t8fx8xpLYhImr8//hpvrPiUdDKFkGAaJk2JBJZlgFchVZMCK4pySqwrQ9fUOqbPmEQQqQGrmaH+EZQI8A2Lp19Yy5LD7uLhP79LQ3cnxUJAbSzFb26/gGde+THZQhk/kUAKDSENjGicsUyGyZMbQGm89NKHRIG1H+0CFG+8tY79d5vMy3f8gWre4v2X1/DNSSey4ohvs+6Ht/PEbX8hLl0q1TJOpUhQLRF4DkK5OK5LYJrs2l6GdD1RHKSpEeSGKOfGKE7qYmarBX4eYZiAz6LpTYxlxvD9AMiCnyWailLo38Z5j99MLBXgF3byj6feZvrcfZg0ax8uOXsRv7/tVH5x4zFodpS3V7zF0QfO4tFfP4yIx3EzRX54w9cp7+qhrq6RVFSQrG0mKI7QP5xnYNNn7Nq6iXuuuIGr9l3G78/5AeuefS7seMBkPJPn5t89w/5TT0DW1GMbJm6+yLPP3ko6VYdhmSRicUQQYJsS369SdUr4fhnXddCkxDIthBbBtEzy4+OMZcfxvCpe4FPbEENoOpFYnMHtPUSlT7aYQwlJ21APfXMWsrYCmmUg0DE1i2gkDp6P73oo38HHR+gaUrfpmjad7nl7015jcMZZx7Nj3Wqshk72W7g3UyZ3cczBBzMlJojndlBnQvceC3jls+1fvov4nwCKAL//88uMuwqnVGBueyN92QwzIzkOOuU0Dl3SStEtI2Nprv72KbSnIsxobwlFiZRHxSlTLuTIjI9SLBbJ5PLki1UqjoPSbaSmU5NI8cbL7wAanjvAUYfvzX4H7svTD95GvLaJqutx7vkn8fNf3s7pJ52BMGvIjI4w3rOemQmDX3z/DEzbxi9nueQ73+Heh37J4qUHgWFTzW7msSeux/eKIA1GKx59/SOMDY8hpE7UlvTuHGNq1zG4TpbalMHGD2+moscxo3HKrkfc1Cn5PuP5Et74IH29m/CFYkfvTpABCoEuoK9njM49p7Kubxctk1onPMUq6Aq7WqEpauE5E3NtvkKTPjHTxA0CXEVYySUGWoQpnU2cuux3BIkO8BzOufx0jj33GHylI6QgaRsUHI9KYQT0CKXcMLYepijseJw7Vj1PIi6RJFn72idcc+UdpFNJkjUpiCdoaWnimuuf5eClP8aMxnGrJSh7xGyLYrVEJBpD1wSmZYZ9pBrE03WUKwmeeuEd6tINHHzUAQzthMGBHMWP17HlXx9hJdq4574n6Jk1g2cWLuK5GfuS6ZiPq8col4s4VQc/cPB8D88L0KSk0TCZPrkORBI/P07gV8O+TsvGb06x7J4boKqDGfC3X/yZ3970MNFoZKJYokMxS0L32FxwWLr/mfjCQPMq/G3l7QxnCriDn1L+60oe+9FD7HvgHEYHR6itq+Vnd3+d6/9wEX62DyIJIt4of3vqajauXs2D990AfgkZSXHb7d/myP0v4NKFy9j06irk5FkMztyX8dkHUnED7KjF6EiGUsmhXJY8fcty7Ka5IB2cwjA3/vwqVKCoSVr45RIN6QhmclI4MmvHMQwTCRRKZQhMDDOKYRloGrjlIiLw8dwqCkn3jG5qa2tZMmcf+j5bj0/Avvfdw3ZdA6Xhlkso5eIFLr5bDtufqmUC3yPwfHxfYUVizN99HxqSGocftjfju4ZYs3E7SdPmjGVH013nc0BkhPqoTWPSpNYWrH3jVf6Tri8dFG95arWoGR+luTFKoQwz95vMYZccAfk+nn/kdTLjGV5++W2aJ89gXrNkTl0ES9epej6672FrOhGpE4nEsAwdSUDUjmFbMYxYmnQ0yr9efw9QGGaC8795BksP3IMrb7iH3JiLoZvMXLgX46Of89yKV6ltaEE3DLKZIs898U+8nM+bL7zH/IYlnLPvQVxy/Bks7VzK9d+/DSvZiBIav7v/Wpyqh51IU1+fprahDl1o1Da30BGzMZyAu//wBs1drZQy45z3tZkUPJOq5yE0ScSIMHdWN/nxDJmhrfSsf5vulihBpczQ4BC7zaznlluO57Szj+aFf62m3WQidyhYuHcXt/3lGrRGm6dW3gWVLOiKqdPbKVcreL6LZSVQpSEY34qfG6ehxqKYk5y46GJ2ZobALfKrm0+mY3Yr+IpkcweqWsaKNoDyaWxpZMbsTtqmtvPnFbdQnxoFLc5vvv9TDj/4IkwzRtVxue/hS9lVzCJ1m9ltrfzghu+iJ9NEWrohMpNHfvEE8XgUXQub3yN2BEPTmVEfpammBctMM7e7g2nT6mmdNI2BLZ9hSZ3itDlM+9aZQJV1n/bgWwlcPYKpm+goEjGLqG2jggA0i1gsjmlamIZOl6fQ62N4PWtItLQilWI4U2T//Zt54elLoVrBjSa56YQf8NtrHiFob8O0InTUheF/UPHYd7/ZqEQr69f2cuIxV0EyQVODztmn7EVPf5btMk61ElAbrXLogddzwRl3c+60H3Be0wXc++inGLaLnmxh7rxGzjl9ERce/A0evuN5kBYzF+zDG2ufpHHZkcSW7Meyy07n6CPnkR8ZIuqWaY1ZTOnupCYdo1w/iQdv+xvXLD2JsX4XMz4JghxVp0TPrjEqhSKL990Nr5AlHqtht/m7sdf8SYxly3R0dIIMqFQKSCmoTaaJp+qIxePMmbeItrYmErE0Y2WXxJRpnL3v8Yx9+DGWbfDsE7+noSGJbZh4rovveriug/D+TTji4/vh2OakyVOYNWc6HdYQnc0pHr/3dgaK0Ne3hfc+3cjsrulU0VnYEceKJjlhQQu3LX/xPwoUtf+Iu6gWf3T0MYsZGiqw+dNB3nttK6vfeJudFUnZFdQ0NRHTFWdcdBYbX3mGXMlnR7aEJkUoqCM1ovEUtmmgCzHR2yZxXI9kTT1yfBdnX3Q20ghoSKcp5ItcfcPvaK3RWLzkAEBx7QXfITNawozGUErhWnGmTuvi0h9fy4n7HYVW10jPcJ6tOwYwo3FWvfUBF337MgzTp7Gpna5JCda/v4GlR+3PkkP2Bw1MO0E6lWDs8828+Px76IUyi4+azx6zGvh0Y4ZSRaclprPXPlO46y9XMr71UySQHxvj5JP35vtXncIFZy3h6OP2ZNKkBFs+H+aBB18ksq2X1Y89R7Hq0dDZwu57LeBrZx5NIuGRyzo8fOcTXHLC96jEa4knm4hGNe69/yLMRAz8MosOPIhP39mEIQV/uOZPTFs4gymz57DsxLk8/si7GFaMBXNbWbp0AYl0E1Y0zelnH8dZ5x1DQ32CsSHFt776X/z5r+8QS6RQSPZcMo/vfP8o/vXMe1B1uPupn7Bo/90pFTQ2fjbA90/6Fv98+T06Zs1F6PrE7LnCD3xSqszyn9yJtCS/vPdWvnb2MfSv28D9//UTBrU4PoJrb/wWpqV46v4n0ewYrguVShmkwHEDhKYhNQNNClQQGmrUtDluVg0HX3EMZgOgLM46fhbnnDCdb5y8ACOdIjNe5efH/4RVr29kZMp0PM+jJRXj3ocvRLOaMWIpZsybQn6Xg4g08/ZLb/LhB72ccPKh7LOonRefW0NvvJVvXHQwTzy0kk82jGFNmUMhYjLZ1pjZ0Iis72TdG+/jZBwOPepAejZsZfntj/PmY/+kfloDCxfP5KsnHsW82TN48Z4/seKhRwmiMY5f0MgPf3M+qeYO6kZ6cHpzDJo2Q72DvHbzA0TdIh377Mf6dZ9zx11/JR6L0RTVmNTdSmtnG/GaJOdfdAaV8UHOOe90uqZ0Uy3k+eTND3nt5TeJNHVxztcOZc7c3chlCviBojGh8dHWUUqmwUv3PMTue02joauD2d0dPP+PdzFNgaFpmHo4xeVNtOe4bhXbjrH0iCNZUD/AQYccxZbVL7Ft1KB3KItmGoz1b0SMDNCeDIjHBE4hSzIo8Nh7W8iWnBv/U0BR/CfcQl1Uu/nFn11+1etbBxkbKZEv+9TUWCHvnR9F1w3a6iOcee7p/O3nV7D67c9YNerSl6uQtHVM3SSersWL12ALGS6QaaNHY7Q31PH722+kefJMIAfk6Vu3iblzDueX5x7G+Q88C3j0vfYs511yE2NWAr/qYESS3PL9k/jLL+7ho6FxKvF6JOG0gikVfnmUg5qSxKsBbR3N5EezPLFpGw/cexWju4pMtQyyW3aw00rS17+ZXy5/hbaqQ1cqwfefvpz05NnsOfdKDpiV4vA57dzx+BsY+SJ2yScGlHTJdFNHlh18BWlLo1/X+GjqXGpwiOd30TI2xoasxzlXnMqyy07mdzfezZN/eYW2pEG1ayZ9foRidpQ/XraExx5/meG8S8FVNLbXsWTxPO598j1UsYTd08Old13BSRefygfvfsp3L7mPoW3r0DN5Fs/r4qB9ZrPse+dgtTXx8vLnOOubvyIaS6LQiUYjDOddbrnqq5x53TKgFtc1KO/czp3f/R0vrniXYqlM3bR2xgqSxqnTyBYKWFYEzwtDXU3TaTJcMhvWYMdSfO/cY3ngpvtoWDiN9anpeJVhXnrzef563dWsvvdJMnMXsj5fxfV9pAir866nqFbDNphypYgbgFMpk+5bT4thctzpe3PE2V9h8j5TQY1AOcPbT/Vy3dl3EG1vYmOsjcCpYgqfGdEi+yHQd2ZIRqMMVnxkY4y7ilFqTI2+jZ/R3d3Ey+88Sl//OLPmHc8F+/2f9s481rLsqs/fHs54p/fufUO9oaq7ph7K7R6qB9OGxgHklkUQTixsJ4AUQpQQEERkIJiQCDBhCAYyAEFRkKJEcSQSYTkewEFG0MbGjdt29Vg9VdWrud787njGPeSPU00bbGNjEqkiZf/13j13OGefvddZe+21vt9dfPzzF2jf8yZMVSOlIBptsnTlAuvCc1eqmSsNJof/0Q7YPnKK03HF9ksXOPnGYwwODfgvv/0kx4+vkasOG1cu0S4LdsuaKXDH2gCV5XT7S5Rxmzc9fDfpxz7GoZUjHPned/B9P/nrLM71iYY3GO9PKNOEv/b2r+d7/s47Of1N38SF567wP3/6fWx/+Pe4ISzPrt9OVuQcn4y5PJrxLglXHIy05o/bMb6/RmIzli5d4T3v/SG+9V/8G372X/5zPvq7n6XMhngvyKqaYjqiKgv2hwe8413fzZuOdVmzT/P49/9rfuGHf5iXJl0qb5BC0gkEkTKcDq+zV8LRuRZPPneZ33xx63M7s/Kh/28U/0z7kb960j/6dY/zmReu4IKYQeBodee4kEe0A4kTNW9+4ARvffx+fuq7v4dPvjLizM4MKRRRIGm3e8x3e2RxF+8scZRSlAUPHu/zzY+e5vd+8wPsbo8JyoKhrdnr9lmqZxivkYGmN9/m2e2MldUjhFJy/vpV3qgzhjVUy8eYzTKkrUiSgEEU0vYF1cEB9yaC0SSnFpJzQjHa3GEeuDuQHG5FTI3ljPFsdBdZiBTz0xF6lPETv/VewvUef+uv/BgqChg7gY9iQiUJtSY3hn6c0NYagaevNVetYzto49C0fMGCL1kXOZONy2xNKmZJTHxoiWnYZSxiwKPKMeHeddq9uQZ4ejDjvjtXeeXVS8zSAcV0Ri8OOZkf0P+Ge3jnD34HN7Yrfvof/wfWDi9gRrsUwwnLPqN0sFUrTNohjmOk1IynGbV3vC3NGbRSVu49xsbLV5hcvE40mGfS7nCdlEvX9xm0YuzyGlEQUFYlgdQI6QkEIEMQlsV8m6XhDjfW7mQ2t06V1bz3xx/nwz/+75g+f4Hynvt4iZBJViFUg1mrvaKuKoTWBGFIMZs1AAOt6LU61KPrVFvXiUdj1vot2q2IzfGMLK8pV48xCufA1WCbTYlBts+pSNNLE3IcVqfMfMnLwRKj/U1akx2GwzFBEPAbv/IPGCYxv/BDv8L2oaMIBzqK8V4QK0U03UEnKcY5lKkYhJJxUbMrE06sLfDmRx5g47NPkVnL3JETXNs4x7Ube8RpzGg0oiUFxhhcPmnQaUkHIxUPPnQvk3LIzlPPMr56nWr9MCOjaccRVoWobES+v40pMpKkze31lLsWF7jWW+IV3WZqPKKYcGowz3g84WSvzXQ4JIpCPpV7xmWBEJJB4HnH5nlu/4a38F0f+TgP3nsfSWdAWRUc7O9TljOyLGNheYXvfPtbub06w3f8wx/h0x/8LX7jQ5eoRIQVkkR6Di91aYU1cu8q3/vNx/mvH3uGH/voWcEt1m6pE3r/97/NP7sFhxZaFM6xaxM6i7fT66UEwjHX9ty1usCNF5/k9z/4IZ44n/PCxNCKNKHW9LrzKKkJOwOidhvjPPXGGeKsZLB2iFArjNBUUjOUKbKeAg0s1lc5trOMqWtagaJ0DjM9gPkVxnv72DpjaXmFVVkQzPaZ+QDXmmcsE8psjLWW+UAxzSbMyoo0aSGUZJYXRDpAekeoFVlVs+xLVq9t8u5f+xFsZ56f/cUPs7W3hQwClI4oqwJnHGESEydt4iQCJKaqGkZhGOK8Q9QlOgypi4zA18xMw2SMWy1q43G2RglQKqQqMqJ8wum7V3jh3A32xzkKh5QgpWI+0fQnY17ZGfITv/6POPPKkM9+4hW2r10Cb+imMQ6BdA4RBozHUyZZgfKSpJMSRwG9esq8q2nPd7lRKcJWi83RlP1RAd6y3k3QSytYITHWolSId42KXxRFTKsaghZhKyWIW5w4epjHjszY+d0neeqZDSa3n+QgbBGGEaPxCCkVUdTGeo9xDZygqht2YG1qwJMkKdPJFGdKYi3o1BNiHCPrqdMBFZoo0JiybGqIvUdK3eh3S4mWmoXBMmU1Y7x3jWp/F4FD1hWtOmdlVPJLv/z3+KUPf54nLo3x6TxaK5RuNjiEK/DFhDTpkedZA+2tK5CKu4+tcPqh+7i2tYU2lhrF1Vdf4mDzgBP3PEhdTdkf7lOXNfuzDKxFCY/SmpNvOEk5m1Ebz/WN81AacucwQmDR6EAiLMTaE3uDCCJ2rKJSMUpAGERNuV5V0WklZFlOVRWEUYSzdXOOHtI05GQv5G0XX+TluMfq33wX/+mDn8AUM7JsRJHnZFnB49/4MN/+4BwPPXiU3Y1z/OGZjDO7Ed12wjSvscWEhUTwlgdvh80NjqWGb/n3nxaT3NxqNvEWiSnebN/zrfM/efpITC1C5paWCI3lzQ/fj006uHiOuN3mTacWOf7A13Nw9vdYixwXdkr2KgiVopjNSOKQxQCMqYiShIkMCdePEna6uM6A0jqqsIUMQwhTSq9wQQLpPFpwExfVEFl8kJCN9lhd6lEWEw5F8NgdXZ4/v0NLW2odEoQxRZE1Bi/P0FLhRaOhHEmJwoMUdFstat9AXtP+PHppgf33f5CXN67x9PYunaRFN0lRuuHcSa3o9PoEUpKmHcIwvgkLrbB1RV2VSATOWpxzWJpkdqUT4igmkDRUFu/xWc6Cdjz0wDp/8KmXmNWGSCvUzTgcspE6v1I5TjzwMM/95kfZef5pdmSbanZAOwoASRyAFaLZbUTS6vSY63epipxsNmNUe67mcHFUMM5zdoYFZWVuFsYJUmVpL6yx7RRWqQbdlXaoggSnIryOUUGIrS17w31GL32C6Uc+wWWnubp2nJ08R+sAYxxFVeCMwXlLWdWo1zh/UuKdpy5nqCCirguUlCglMQbqqMNYJVjdxiuJspZAS4xpRLO8dyilkcITKI1zNe12ynwnZnzjCqIsEN4RDg7RVo4j6wmvfOCTvHRti/lBhyzuIm7W6iupcKYgwLF05BSjvet4oUCHhKGik4Ssry5S5lO8d41G8851cIpub55ZOcOUJXhLKn2TDpN0cN7RThOyYoI1FfODHkQhSkmsg6TbZ2IEgfAo1UguoDRGR4ggQnlQoUYIQbuVQgNOR0qPNRUKUEISBAKNx0chFxeWmbmS3/nAR5ChIhYOUxuqqmR50OPvfvt9PHRPj6Pf+Le5eObTbJYLxOk8aafLXUfX2Nu6wfH1HneeOkJX1OijA37tA0//FLdgu6WM4jPn9n/y+04H6K0rLMVDQgr8jeeQF/+Qevci2cXPcunMJ3n1iQ+xfOIukuICYlyzMXYI6YlDhTcltakJTUHoMqpsyurqCh3G3Hv/Izz/ykv00ohWrOlEiuV+i+noAFmXCFujpEcHTWrKwnyHLMtZCEuKvOTb7u9jigmjYYnqzJN0EgSOwUKf/b0tDi906EWCXrtNNw1YmEuJAsF8GhFpz1yqCaUFUzdo+BNHqaYH9LpzeFOTJpLAe1rK0RaStrSErialRJYzEuVJdEAioBOFTIZ7yDrH1SUhliKf8k33HmPnYAh1QScOmdOWNxzr42zJp598GR0GBEq9DqW1njAOQAiWDq0Q6YBzuxN6y+uoOGU2HoOEVhTidEQQhIRRQhTF+DrnYHcPY02D+AeEdDcrjQK8b/qyKV0WCO+IsEyQaCXwQtHtdslmObUxlHWOM008zjtLZGu2F2/noL2AFjVKByRJi6KowFtiHeKtJY4U/e4c2IpIKSKtSIKAbquLybObsFSLcRalA2bZjDBJ0ELS6c2jgwCBJAgCXG2xNKlNQgqiMAGbMT4YY+qclU7I4fU1ct0mWlhlMarZjUPm+ylCapZ6Kee29omShKTbZ3Vpjd3rr/LAvQ+xPz4ApQniCOMFqws9Dq/cTj4dUZQFVS2Y7O0TKoVQKbaeYAwNRmySocJWI4GBoTMfUdQ10gmE1Cz0U9bX+8xJw/WZYeXoPVRCoKMUpTylSiDqUBuDCiOiuA1SkcQpnf4CFst8bw7vHXGsSJOAQbfF0kKPuRAiV6JaKcvrq3DzIWywZEXFu79unbeciuiu3sl//Cf/jJd3BBf3C5TN2bxwlovnztIPKkQxZvfceR5b0/zMx87x/Lmdn7oVkrVvCUrOl2tnrxXio9sjv3iQUBWeYQWHl3qktqCvDGsrMYuLPYIqYXM65NDpB/j803/Io33JizNoJ4pOHBBqQTdRLCy2GE5DFvshqooI3YzVQ0to3wjWR0lEFGrk+iK7O/tURU0QCZT2iDCm12nRObbCbPMC0lrSRICOWep3sN0uaTth4hVJFKDLDitzMf1ORFZALcAasD2Nt74RTZeC2UxRVQ3wNk0VIlojVhq5NSLudagqgxAOZwVhHCNlQBBpSquorGuSnhFILfn73/k3qARsXLnMZDJivd7h9J1L3H+4x1OXbxDjuLLlePqZC4yGM8IobKjeDQi6icdREwYhtatZXV5i4+yLeG84mGaEcwl1XYNXjG1OqA0CR2EcxlR4IfDagg/RWuOdRxpBg4RsqNevyxJIahUQdvqsLx0iihOUDrGTXVaOLpHlJVY0ifBBHDOdjJHhHchyStxOiSm4eHWbdtrh1JEjjLKcKE5I2/PEgeG5Vy9xaHmN+cEy0/E+0+kEU5fIbodEeZwrSIKQKNT0F+5gNMup6ora2gau2+40Cc5SIIWiFcd0Wy1UEHHhwlmG2nNb2mKx0+HwsduZXciQOHTnLh69ewdfW4YTy37m+O7TS2yZNkYHrCzOcWf6KM9+/n/xjre+m888+3l02qWuaxZ7mtoYpIxQgSSVAhkpgtIhpMPWYMoKU1bgQAaaLJuysNhjNpzhrAEVIFyNjwNwNfV4ymxccOf8PNLMmEwrolCg0DgVNNo5OqAdRxhbk8QhrSTBVAlxFGFMSYAlCTxpGtIJFaExeBWC9JQ1KKVIheNgmPPm4wPuj/c5fuQufvU9/4o/uOaooj1y50m1QggYJIqdkWDsPa1JxtWHl/lvv/O8AHcrOoq3VkxRIDjRV/4/f/sSdbzGszcswxms9FLiMMCLgqNHEhb6js1hTqza3Lh4gd///Q1eyiXaek4f7nLyUIcH72xzeHWJ7lyLJ851NYJ0AAALrUlEQVRc4uu/7W3s7F5k5fRjbH36DEUtUUlMP22DcwQPPsJvv+/nWFxbRwtHGgqGO5vceXKZf/vB5zFBys//2g/wRz/3qxx/w3EiBe1Wnxvjknw24+XL23zLcc1nNsacvzHj6ggW52KUavRb9kY5de2QOqSoIath8yAnd562Viz0QtbnFAJPFCvCOMDWDic1t508TG0kWZYRxjEH27tsbk/pRY4TK32kUBRoOoPDnL14g4+fP6DX7vHi2ZcZjjOCuIHJIgXSWexN2QIlFDqOG+8vjRjECdevXQWpqa0jTROGB3s3i/896iam3klJLCUVHqEUOgooZgXeOaxxBKHGAUIFDZ+QBo7bTiPuPrGEkx6cxHpLr9sjDGJimRO2WggfM5lOKWrH9Z19rLEcXl+gqD1eBox395BBSBLHpEmK8h4nJbPhPlIKJllOHCdM85wiy6mspT/XpdeOMWVJaWpsVWOsYTLLwXsyY5DOksYBi50OxnlE0KT3zKea0WzCVg4PLWuU8aSDJUZ0G43qqE3LjWmVmxTZjF4r4OyFXQarR5DxPPNpzKmVQ/jJdY7cdZrPPPsCGsON3W2mq3ehojajg31mVYEvLZevbBCJkDAdkI23mGQzROkwrqI7WOPq5nWOnVhmurOFkRqpFVGY0u1GdDuKa8+fZ2Uw4JF7lvnrb/5mPv7k03hXsbs/ZLEXYZ1lNitII4nCcftSH1/nPLNxrXlQ2pqVVHJby7GZC+ZSzTSvWejE3BgWPHdtzG5heXFrxk4Q8PgyvOX+Ba49dZFP7Dp8GhHEIc46giYexaiwODTH+jGP9QQ//sd7/NFmLrhF2y13Yt0Y3vtW7aNZDy1TRrki0JJ71lM2ZgFht0O74zm5ZvAHQ1rC8tQzV/nsiyUf3PS8sScYtGMeWG9x350DVgYxo5lhZS6kFQgOJlOWVvrMKs+0MCRpizwT9BZTXCmIlSVemUP2A7ACLt1gkuUEb7yX8IWzyMMrTDa2sECsJMNJzsXtGaNJzVo/xMRtrPDsj6rGsDlBXdcIPKNhQVZUREqiFExyx+Z+zqCnqSvH6nLErLA4L/FaM9dN2Lg8QgkYLLUpak8YxPRamu2tCShIWylPnz/glaEnXV3H+YCLL51jd3uXINJ41WD1G7UngXM1UoY4XxMEGhlEhEjqOqPX6jIajzC1I2nP42VFNZwAnjC4mTAvBdILZBThsFgvKMoS5yxBFOBtjaklYaipjEZI04g+BZpjK/M8eHcfW3parRhvDaYyzKchg7ji2tRx9NAhrFBU0wlnz28SaEk39uxMS3pRRFbUHD08oChrep2YdhoxyQv2D3Lm57ts747Z3DrAOMu0dKSxQiiJlBFBKEgCydb+mPvvWGc0nhIHgu1RST4rme9F5FaRxrKRFRBwZBAQUrOxlbO9X+J8Tac/z1waMp1lrCx0WOwmuHyKmx6w0BakrQ6ViahsSGYsSinm4pB+aDBOIsKQzMAT6hSjzJBnE6yvmO6OuHLtEku9VXIvKGcjxpMJUnhMUbC0uMS5a9c5deoYu9evIIIYoUICVdFKUgb9kBfPnOfxu1d5212L7IynFFaw1Ekp65pJXuKdYFyVTbVXHBJQMTzYox9IhnlBpBSplmwfTG6uJeXNMnvPbil4ZViyOan4xNaMNx3u8JYjKQ+riheu7HO5DtgTmm6kGNeOQx0JteeykUQe5rUhN5pffHZ4yxrEW275DDAu4Ic/bMXPvHHkL+cZ3ghOdgNerqb0WyFVMcGVMbMgoR0lfPyZfe6b67A5V/JoJbiaQWZKNg5Krm5NefPhiH474Hefs1wfVjx8vMML58b05kL2JyWBaCbw5U/N8DLk3mNzPPnxszxxecZFG7A0l/KmuxeYfvoJPnfugLp4nu9/+ykeua3N7vU9Zld38bnhDcsx01lGWIxZXU9xiaeuDMYaStV4ULYPQjcbIsWsZiRLTg80gXLoQNA+GiOsQLVi6sKyuZXz8uyAmdSs6YhDS10Ekr3tPXQ75cVtw8VrhkosMPIZF55+FZtloBVhFGHxN0nhTQ01NOtmYStQkqoyhEhagwE7OzP2xyNiHdKKFDaQiMpilGo8ReXRWmJqg1KSvCpA+EYcynuEChsadhTf3KAwJEnQkLYdzUZRnrGQWS6MK+ZEh66Gw/2ISNeIIqOaTqmvjXju8gFPXRlyfr+i9J5EQKQk1nvSULFw9jxH5iJy6zh5qA0OTqz2GNQVo9E+cVEwrSyH04CFJMQbx34+JvYBw1FNWFgenas4oOT5K0Mm2zlT45gcKDqJYrs0DTxYesyOIpGQKsHAO4z1jK5f5XN7BUdShb8hmASCFE9Ve6JBgG4NiQPJShoyFJqFdkxkLecvFlw4MIRK4VSE/LpHKLevgM1pxTFjYakKg14OSPDkuUZrAVaDKxFCYb3FAd7WOBWArwhFjfeCYpYzLkrCbIgZCdy0YjguKPck07xkVhkOippIKtIATKSwVc3+tOJMWdISinYoibB413hM09LilGSndGxMDRPvuTCz3NcPmZeGw3nB+y+OuLeneGzFs+NgVHjKWDCpPZGDfGZw2tGtLL/80vSWNoi3pKcIjWToYsT+P71Dz29MBN4J5gI4kgTcP4i4VgpMHDCrBUPjCKRmXQx56mpOaeDJkedTUy8kIEUTyDX+S199O9TU1lKbJu5X268t8Ktu8j2UAC0FzjdaHs436Ia/eB9wU0byi9tCuPDfB93WOweLbepihjWeuixRuiGW4CyV9Vjvm8eebwSK8J5Qq9dJ2cITBZpWdw6CmIPNq/Q7LXzcwu5tUYmAVqdFPs0oPVhXN0JQtUEKhxYSdIAWklFRgCvpdOeobRML81KhpMDWFUmaMitmnN/e+aLQ+mteWe2/3BD96u5J8AWiR957jH9dd+4vE87XQqAVaCWIA8XutP6qPhcKqHwzNuJAkNf+T6Jo95+6g+9693ex+cpZBBVhd4Ur589y/sI1Tt11isLXzCY14+Ee3pTks5yV1cOcv3KR++69gxtXLyJU0KT/CEuapnRDweeeucTLe3tiZuyXnOj+zzUF/iseeeOc9o8tR7RDQeIh9I6eUiwEjn5HgpGsdySvjgzXMkfmHT3puS30/MBzNecybtlY4i3rKb42oLcL0f+5l8zPf+8R9aO5F2xmEo1hfN3gJAwri/WSxRQuzTzPekFPR7wwM3w2qwWNLHkj1fllf0gwLV83Wc76r5GELpody5u/97Ua1j/dB1/+2F69965gUnlfFiweXqbKcmRbI5WiqiowEiVrJI2SnkeCrRr9Z9Eo3EnnqB0kcYTSnrqq6IQRKo3AWHSnw+qhQ2xv7jZL8CJHSUWVTwiDECUC4iQlSBJ8nVEXkKMQwpMmEaNaoJXB+4bMbK2gF6Vfsm///Pvkv2Lfv/ae2n3x/fs/Mf2M980usPFMS/dVm+rq5hssMPszFv+29TVuXH6V0XiPRCtkr4l1IgRWOGxVY4XDeQFOo26WrQaxxjiD8QGJTBo9Z2qMkzjXqDH+6b5szvQrj0j/VR15fmRFr6P9SmFwOB5JPHe2PFFpcBNFOxZUheRoAB3lm9zQ2PODz9acy9wt7yXeskbxtVuxV/Ge950z7/lSA/C11/5ycg7+LzwFv7aJ+xf3lF+7xtf+9v71Ce+BzWosKm9/Xm4FPzq30G2gn0qAMAQBRAiqukQFEmscTjVypBiLwSAFhFoSpjGmMngc3aUBgfDUomb5ttuY7uySFRl1VeK9xDvHfNTCBgGhUlgaik+UdKiMp5oOybKSbjdAaxC20cVuto8qgqjNfJgwrIuvetniv4olTSMdelP43r/+Of8Fnuhrr33Z90j1J/99YV+LmzKef/Yuyy91bje/3H2F4aAE3PuGN7B96QKBsPigy3Q4pK4qkiDC2aoR6ioyvHOUxYwoUFSuopUkFEXZPDS1QmLxVmKNYWqhrmpq7/6vjU3vPZ+8MvqTS/7AF1y/EPXrfepf7xvH/1vtfwPeb8xX1hSFXAAAAABJRU5ErkJggg==";
    os << ") 0px 0px no-repeat; }\n"
       << "\t\t\t#masthead h2 {margin-left: 355px;color: 268f45; }\n"
       << "\t\t\t#masthead ul.params {margin: 20px 0 0 345px; }\n"
       << "\t\t\t#masthead p {color: #fff;margin: 20px 20px 0 20px; }\n"
       << "\t\t\t#notice {font-size: 12px;-moz-box-shadow: 0px 0px 8px #E41B17;-webkit-box-shadow: 0px 0px 8px #E41B17;box-shadow: 0px 0px 8px #E41B17; }\n"
       << "\t\t\t.alert {width: 800px;padding: 10px;margin: 10px 0 10px 0;background-color: #333;-moz-border-radius: 6px;-khtml-border-radius: 6px;-webkit-border-radius: 6px;border-radius: 6px;-moz-box-shadow: inset 0px 0px 6px #C11B17;-webkit-box-shadow: inset 0px 0px 6px #C11B17;box-shadow: inset 0px 0px 6px #C11B17; }\n"
       << "\t\t\t.alert p {margin-bottom: 0px; }\n"
       << "\t\t\t.section .toggle-content {padding-left: 18px; }\n"
       << "\t\t\t.player > .toggle-content {padding-left: 0; }\n"
       << "\t\t\t.toc {float: left;padding: 0; }\n"
       << "\t\t\t.toc-wide {width: 560px; }\n"
       << "\t\t\t.toc-narrow {width: 375px; }\n"
       << "\t\t\t.toc li {margin-bottom: 10px;list-style-type: none; }\n"
       << "\t\t\t.toc li ul {padding-left: 10px; }\n"
       << "\t\t\t.toc li ul li {margin: 0;list-style-type: none;font-size: 13px; }\n"
       << "\t\t\t#raid-summary .toggle-content {padding-bottom: 0px; }\n"
       << "\t\t\t#raid-summary ul.params,#raid-summary ul.params li:first-child {margin-left: 0; }\n"
       << "\t\t\t.charts {float: left;width: 541px;margin-top: 10px; }\n"
       << "\t\t\t.charts-left {margin-right: 40px; }\n"
       << "\t\t\t.charts img {background-color: #333;padding: 5px;margin-bottom: 20px;-moz-border-radius: 6px;-khtml-border-radius: 6px;-webkit-border-radius: 6px;border-radius: 6px; }\n"
       << "\t\t\t.talents div.float {width: auto;margin-right: 50px; }\n"
       << "\t\t\ttable.sc {background-color: #333;padding: 4px 2px 2px 2px;margin: 10px 0 20px 0;-moz-border-radius: 6px;-khtml-border-radius: 6px;-webkit-border-radius: 6px;border-radius: 6px; }\n"
       << "\t\t\ttable.sc tr {color: #fff;background-color: #1a1a1a; }\n"
       << "\t\t\ttable.sc tr.head {background-color: #aaa;color: #fff; }\n"
       << "\t\t\ttable.sc tr.odd {background-color: #222; }\n"
       << "\t\t\ttable.sc th {padding: 2px 4px 4px 4px;text-align: center;background-color: #333;color: #fff; }\n"
       << "\t\t\ttable.sc td {padding: 2px;text-align: center;font-size: 13px; }\n"
       << "\t\t\ttable.sc th.left, table.sc td.left, table.sc tr.left th, table.sc tr.left td {text-align: left; }\n"
       << "\t\t\ttable.sc th.right, table.sc td.right, table.sc tr.right th, table.sc tr.right td {text-align: right;padding-right: 4px; }\n"
       << "\t\t\ttable.sc th.small {padding: 2px 2px 3px 2px;font-size: 11px; }\n"
       << "\t\t\ttable.sc td.small {padding: 2px 2px 3px 2px;font-size: 11px; }\n"
       << "\t\t\ttable.sc tr.details td {padding: 0 0 15px 15px;text-align: left;background-color: #333;font-size: 11px; }\n"
       << "\t\t\ttable.sc tr.details td ul {padding: 0;margin: 4px 0 8px 0; }\n"
       << "\t\t\ttable.sc tr.details td ul li {clear: both;padding: 2px;list-style-type: none; }\n"
       << "\t\t\ttable.sc tr.details td ul li span.label {display: block;padding: 2px;float: left;width: 145px;margin-right: 4px;background: #222; }\n"
       << "\t\t\ttable.sc tr.details td ul li span.tooltip {display: block;float: left;width: 190px; }\n"
       << "\t\t\ttable.sc tr.details td ul li span.tooltip-wider {display: block;float: left;width: 350px; }\n"
       << "\t\t\ttable.sc tr.details td div.float {width: 350px; }\n"
       << "\t\t\ttable.sc tr.details td div.float h5 {margin-top: 4px; }\n"
       << "\t\t\ttable.sc tr.details td div.float ul {margin: 0 0 12px 0; }\n"
       << "\t\t\ttable.sc td.filler {background-color: #333; }\n"
       << "\t\t\ttable.sc .dynamic-buffs tr.details td ul li span.label {width: 120px; }\n"
       << "\t\t\ttr.details td table.details {padding: 0px;margin: 5px 0 10px 0; }\n"
       << "\t\t\ttr.details td table.details tr th {background-color: #222; }\n"
       << "\t\t\ttr.details td table.details tr td {background-color: #2d2d2d; }\n"
       << "\t\t\ttr.details td table.details tr.odd td {background-color: #292929; }\n"
       << "\t\t\ttr.details td table.details tr td {padding: 1px 3px 1px 3px; }\n"
       << "\t\t\ttr.details td table.details tr td.right {text-align: right; }\n"
       << "\t\t\t.player-thumbnail {float: right;margin: 8px;border-radius: 12px;-moz-border-radius: 12px;-khtml-border-radius: 12px;-webkit-border-radius: 12px; }\n"
       << "\t\t</style>\n";
  }
}

// print_html_masthead ======================================================

void print_html_masthead( report::sc_html_stream& os, sim_t* sim )
{
  // Begin masthead section
  os << "\t\t<div id=\"masthead\" class=\"section section-open\">\n\n";

  os.printf(
    "\t\t\t<span id=\"logo\"></span>\n"
    "\t\t\t<h1><a href=\"%s\">SimulationCraft %s</a></h1>\n"
    "\t\t\t<h2>for World of Warcraft %s %s (build level %d)</h2>\n\n",
    ( sim -> hosted_html ) ? "http://www.simulationcraft.org/" : "http://code.google.com/p/simulationcraft/",
    SC_VERSION, sim -> dbc.wow_version(), ( sim -> dbc.ptr ?
#if SC_BETA
        "BETA"
#else
        "PTR"
#endif
        : "Live" ), sim -> dbc.build_level() );

  time_t rawtime;
  time( &rawtime );

  os << "\t\t\t<ul class=\"params\">\n";
  os.printf(
    "\t\t\t\t<li><b>Timestamp:</b> %s</li>\n",
    ctime( &rawtime ) );
  os.printf(
    "\t\t\t\t<li><b>Iterations:</b> %d</li>\n",
    sim -> iterations );

  if ( sim -> vary_combat_length > 0.0 )
  {
    timespan_t min_length = sim -> max_time * ( 1 - sim -> vary_combat_length );
    timespan_t max_length = sim -> max_time * ( 1 + sim -> vary_combat_length );
    os.printf(
      "\t\t\t\t<li class=\"linked\"><a href=\"#help-fight-length\" class=\"help\"><b>Fight Length:</b> %.0f - %.0f</a></li>\n",
      min_length.total_seconds(),
      max_length.total_seconds() );
  }
  else
  {
    os.printf(
      "\t\t\t\t<li><b>Fight Length:</b> %.0f</li>\n",
      sim -> max_time.total_seconds() );
  }
  os.printf(
    "\t\t\t\t<li><b>Fight Style:</b> %s</li>\n",
    sim -> fight_style.c_str() );
  os << "\t\t\t</ul>\n"
     << "\t\t\t<div class=\"clear\"></div>\n\n"
     << "\t\t</div>\n\n";
  // End masthead section
}

void print_html_errors( report::sc_html_stream& os, sim_t* sim )
{
  if ( ! sim -> error_list.empty() )
  {
    os << "\t\t<pre style=\"color: black; background-color: white; font-weight: bold;\">\n";
    size_t num_errors = sim -> error_list.size();
    for ( size_t i = 0; i < num_errors; i++ )
      os <<  sim -> error_list[ i ] << "\n";

    os << "\t\t</pre>\n\n";
  }
}

void print_html_beta_warning( report::sc_html_stream& os )
{
#if SC_BETA
  os << "\t\t<div id=\"notice\" class=\"section section-open\">\n"
     << "\t\t\t<h2>Beta Release</h2>\n"
     << "\t\t\t<ul>\n";

  for ( size_t i = 0; i < sizeof_array( report::beta_warnings ); ++i )
    os << "\t\t\t\t<li>" << report::beta_warnings[ i ] << "</li>\n";

  os << "\t\t\t</ul>\n"
     << "\t\t</div>\n\n";
#else
  (void)os;
#endif
}

void print_html_image_load_scripts( report::sc_html_stream& os, sim_t* sim )
{
  // Toggles, image load-on-demand, etc. Load from simulationcraft.org if
  // hosted_html=1, otherwise embed
  if ( sim -> hosted_html )
  {
    os << "\t\t<script type=\"text/javascript\" src=\"http://www.simulationcraft.org/js/ga.js\"></script>\n"
       << "\t\t<script type=\"text/javascript\" src=\"http://www.simulationcraft.org/js/rep.js\"></script>\n"
       << "\t\t<script type=\"text/javascript\" src=\"http://static.wowhead.com/widgets/power.js\"></script>\n";
  }
  else
  {
    os << "\t\t<script type=\"text/javascript\">\n"
       << "\t\t\tfunction load_images(containers) {\n"
       << "\t\t\t\tvar $j = jQuery.noConflict();\n"
       << "\t\t\t\tcontainers.each(function() {\n"
       << "\t\t\t\t\t$j(this).children('span').each(function() {\n"
       << "\t\t\t\t\t\tvar j = jQuery.noConflict();\n"
       << "\t\t\t\t\t\timg_class = $j(this).attr('class');\n"
       << "\t\t\t\t\t\timg_alt = $j(this).attr('title');\n"
       << "\t\t\t\t\t\timg_src = $j(this).html().replace(/&amp;/g, '&');\n"
       << "\t\t\t\t\t\tvar img = new Image();\n"
       << "\t\t\t\t\t\t$j(img).attr('class', img_class);\n"
       << "\t\t\t\t\t\t$j(img).attr('src', img_src);\n"
       << "\t\t\t\t\t\t$j(img).attr('alt', img_alt);\n"
       << "\t\t\t\t\t\t$j(this).replaceWith(img);\n"
       << "\t\t\t\t\t\t$j(this).load();\n"
       << "\t\t\t\t\t});\n"
       << "\t\t\t\t});\n"
       << "\t\t\t}\n"
       << "\t\t\tfunction open_anchor(anchor) {\n"
       << "\t\t\t\tvar img_id = '';\n"
       << "\t\t\t\tvar src = '';\n"
       << "\t\t\t\tvar target = '';\n"
       << "\t\t\t\tanchor.addClass('open');\n"
       << "\t\t\t\tvar section = anchor.parent('.section');\n"
       << "\t\t\t\tsection.addClass('section-open');\n"
       << "\t\t\t\tsection.removeClass('grouped-first');\n"
       << "\t\t\t\tsection.removeClass('grouped-last');\n"
       << "\t\t\t\tif (!(section.next().hasClass('section-open'))) {\n"
       << "\t\t\t\t\tsection.next().addClass('grouped-first');\n"
       << "\t\t\t\t}\n"
       << "\t\t\t\tif (!(section.prev().hasClass('section-open'))) {\n"
       << "\t\t\t\t\tsection.prev().addClass('grouped-last');\n"
       << "\t\t\t\t}\n"
       << "\t\t\t\tanchor.next('.toggle-content').show(150);\n"
       << "\t\t\t\tchart_containers = anchor.next('.toggle-content').find('.charts');\n"
       << "\t\t\t\tload_images(chart_containers);\n"
       << "\t\t\t\tsetTimeout(\"var ypos=0;var e=document.getElementById('\" + section.attr('id') + \"');while( e != null ) {ypos += e.offsetTop;e = e.offsetParent;}window.scrollTo(0,ypos);\", 500);\n"
       << "\t\t\t}\n"
       << "\t\t\tjQuery.noConflict();\n"
       << "\t\t\tjQuery(document).ready(function($) {\n"
       << "\t\t\t\tvar chart_containers = false;\n"
       << "\t\t\t\tvar anchor_check = document.location.href.split('#');\n"
       << "\t\t\t\tif (anchor_check.length > 1) {\n"
       << "\t\t\t\t\tvar anchor = anchor_check[anchor_check.length - 1];\n"
       << "\t\t\t\t}\n"
       << "\t\t\t\t$('a.ext').mouseover(function() {\n"
       << "\t\t\t\t\t$(this).attr('target', '_blank');\n"
       << "\t\t\t\t});\n"
       << "\t\t\t\t$('.toggle').click(function(e) {\n"
       << "\t\t\t\t\tvar img_id = '';\n"
       << "\t\t\t\t\tvar src = '';\n"
       << "\t\t\t\t\tvar target = '';\n"
       << "\t\t\t\t\te.preventDefault();\n"
       << "\t\t\t\t\t$(this).toggleClass('open');\n"
       << "\t\t\t\t\tvar section = $(this).parent('.section');\n"
       << "\t\t\t\t\tif (section.attr('id') != 'masthead') {\n"
       << "\t\t\t\t\t\tsection.toggleClass('section-open');\n"
       << "\t\t\t\t\t}\n"
       << "\t\t\t\t\tif (section.attr('id') != 'masthead' && section.hasClass('section-open')) {\n"
       << "\t\t\t\t\t\tsection.removeClass('grouped-first');\n"
       << "\t\t\t\t\t\tsection.removeClass('grouped-last');\n"
       << "\t\t\t\t\t\tif (!(section.next().hasClass('section-open'))) {\n"
       << "\t\t\t\t\t\t\tsection.next().addClass('grouped-first');\n"
       << "\t\t\t\t\t\t}\n"
       << "\t\t\t\t\t\tif (!(section.prev().hasClass('section-open'))) {\n"
       << "\t\t\t\t\t\t\tsection.prev().addClass('grouped-last');\n"
       << "\t\t\t\t\t\t}\n"
       << "\t\t\t\t\t} else if (section.attr('id') != 'masthead') {\n"
       << "\t\t\t\t\t\tif (section.hasClass('final') || section.next().hasClass('section-open')) {\n"
       << "\t\t\t\t\t\t\tsection.addClass('grouped-last');\n"
       << "\t\t\t\t\t\t} else {\n"
       << "\t\t\t\t\t\t\tsection.next().removeClass('grouped-first');\n"
       << "\t\t\t\t\t\t}\n"
       << "\t\t\t\t\t\tif (section.prev().hasClass('section-open')) {\n"
       << "\t\t\t\t\t\t\tsection.addClass('grouped-first');\n"
       << "\t\t\t\t\t\t} else {\n"
       << "\t\t\t\t\t\t\tsection.prev().removeClass('grouped-last');\n"
       << "\t\t\t\t\t\t}\n"
       << "\t\t\t\t\t}\n"
       << "\t\t\t\t\t$(this).next('.toggle-content').toggle(150);\n"
       << "\t\t\t\t\t$(this).prev('.toggle-thumbnail').toggleClass('hide');\n"
       << "\t\t\t\t\tchart_containers = $(this).next('.toggle-content').find('.charts');\n"
       << "\t\t\t\t\tload_images(chart_containers);\n"
       << "\t\t\t\t});\n"
       << "\t\t\t\t$('.toggle-details').click(function(e) {\n"
       << "\t\t\t\t\te.preventDefault();\n"
       << "\t\t\t\t\t$(this).toggleClass('open');\n"
       << "\t\t\t\t\t$(this).parents().next('.details').toggleClass('hide');\n"
       << "\t\t\t\t});\n"
       << "\t\t\t\t$('.toggle-db-details').click(function(e) {\n"
       << "\t\t\t\t\te.preventDefault();\n"
       << "\t\t\t\t\t$(this).toggleClass('open');\n"
       << "\t\t\t\t\t$(this).parent().next('.toggle-content').toggle(150);\n"
       << "\t\t\t\t});\n"
       << "\t\t\t\t$('.help').click(function(e) {\n"
       << "\t\t\t\t\te.preventDefault();\n"
       << "\t\t\t\t\tvar target = $(this).attr('href') + ' .help-box';\n"
       << "\t\t\t\t\tvar content = $(target).html();\n"
       << "\t\t\t\t\t$('#active-help-dynamic .help-box').html(content);\n"
       << "\t\t\t\t\t$('#active-help .help-box').show();\n"
       << "\t\t\t\t\tvar t = e.pageY - 20;\n"
       << "\t\t\t\t\tvar l = e.pageX - 20;\n"
       << "\t\t\t\t\t$('#active-help').css({top:t,left:l});\n"
       << "\t\t\t\t\t$('#active-help').show(250);\n"
       << "\t\t\t\t});\n"
       << "\t\t\t\t$('#active-help a.close').click(function(e) {\n"
       << "\t\t\t\t\te.preventDefault();\n"
       << "\t\t\t\t\t$('#active-help').toggle(250);\n"
       << "\t\t\t\t});\n"
       << "\t\t\t\tif (anchor) {\n"
       << "\t\t\t\t\tanchor = '#' + anchor;\n"
       << "\t\t\t\t\ttarget = $(anchor).children('h2:first');\n"
       << "\t\t\t\t\topen_anchor(target);\n"
       << "\t\t\t\t}\n"
       << "\t\t\t\t$('ul.toc li a').click(function(e) {\n"
       << "\t\t\t\t\tanchor = $(this).attr('href');\n"
       << "\t\t\t\t\ttarget = $(anchor).children('h2:first');\n"
       << "\t\t\t\t\topen_anchor(target);\n"
       << "\t\t\t\t});\n"
       << "\t\t\t});\n"
       << "\t\t</script>\n\n";
  }
}


// print_html ===============================================================
void print_html_( report::sc_html_stream& os, sim_t* sim )
{
  os << "<!DOCTYPE html>\n\n";
  os << "<html>\n";

  os << "\t<head>\n";
  os << "\t\t<title>Simulationcraft Results</title>\n";
  os << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n";

  print_html_styles( os, sim );

  os.printf( "\t\t<script type=\"text/javascript\">\n"
             "\t\tfunction HighlightText(obj){\n"
             "\t\t\tif (document.selection) {\n"
             "\t\t\t\tvar r1 = document.body.createTextRange();\n"
             "\t\t\t\tr1.moveToElementText(document.getElementById(obj));\n"
             "\t\t\t\tr1.select();\n"
             "\t\t\t} else {\n"
             "\t\t\t\tvar s = window.getSelection();\n"
             "\t\t\t\tvar r1 = document.createRange();\n"
             "\t\t\t\tr1.setStartBefore(document.getElementById(obj));\n"
             "\t\t\t\tr1.setEndAfter(document.getElementById(obj));\n"
             "\t\t\t\ts.addRange(r1);\n"
             "\t\t\t}\n"
             "\t\t}\n"
             "\t\t</script>\n" );

  os << "\t</head>\n\n";

  os << "\t<body>\n";

  print_html_errors( os, sim );

  // Prints div wrappers for help popups
  os << "\t\t<div id=\"active-help\">\n"
     << "\t\t\t<div id=\"active-help-dynamic\">\n"
     << "\t\t\t\t<div class=\"help-box\"></div>\n"
     << "\t\t\t\t<a href=\"#\" class=\"close\"><span class=\"hide\">close</span></a>\n"
     << "\t\t\t</div>\n"
     << "\t\t</div>\n\n";

  print_html_masthead( os, sim );

  print_html_beta_warning( os );

  size_t num_players = sim -> players_by_name.size();

  if ( num_players > 1 || sim -> report_raid_summary )
  {
    print_html_contents( os, sim );
  }

  if ( num_players > 1 || sim -> report_raid_summary || ! sim -> raid_events_str.empty() )
  {
    print_html_raid_summary( os, sim, sim -> report_information );
    print_html_scale_factors( os, sim );
  }

  int k = 0;
  // Report Players
  for ( size_t i = 0; i < num_players; ++i )
  {
    report::print_html_player( os, sim -> players_by_name[ i ], k );
    ++k;

    // Pets
    if ( sim -> report_pets_separately )
    {
      std::vector<pet_t*>& pl = sim -> players_by_name[ i ] -> pet_list;
      for ( size_t j = 0; j < pl.size(); ++j )
      {
        pet_t* pet = pl[ j ];
        if ( pet -> summoned && !pet -> quiet )
          report::print_html_player( os, pet, 1 );
      }
    }
  }

  // Sim Summary
  print_html_sim_summary( os, sim, sim -> report_information );

  if ( sim -> report_raw_abilities )
    print_html_raw_ability_summary( os, sim );

  // Report Targets
  if ( sim -> report_targets )
  {
    for ( size_t i = 0; i < sim -> targets_by_name.size(); ++i )
    {
      report::print_html_player( os, sim -> targets_by_name[ i ], k );
      ++k;

      // Pets
      if ( sim -> report_pets_separately )
      {
        std::vector<pet_t*>& pl = sim -> targets_by_name[ i ] -> pet_list;
        for ( size_t j = 0; j < pl.size(); ++j )
        {
          pet_t* pet = pl[ j ];
          //if ( pet -> summoned )
          report::print_html_player( os, pet, 1 );
        }
      }
    }
  }

  // Help Boxes
  print_html_help_boxes( os, sim );

  // jQuery
  // The /1/ url auto-updates to the latest minified version
  os << "\t\t<script type=\"text/javascript\" src=\"http://ajax.googleapis.com/ajax/libs/jquery/1/jquery.min.js\"></script>\n";

  print_html_image_load_scripts( os, sim );

  if ( num_players > 1 )
    print_html_raid_imagemaps( os, sim, sim -> report_information );

  os << "\t</body>\n\n"
     << "</html>\n";
}

} // UNNAMED NAMESPACE ====================================================

namespace report {

void print_html( sim_t* sim )
{
  if ( sim -> simulation_length.mean() == 0 ) return;
  if ( sim -> html_file_str.empty() ) return;


  // Setup file stream and open file
  report::sc_html_stream s;
  s.open( sim, sim -> html_file_str );
  if ( ! s )
    return;

  report::generate_sim_report_information( sim, sim -> report_information );

  // Set floating point formatting
  s.precision( sim -> report_precision );
  s << std::fixed;

  // Print html report
  print_html_( s, sim );

  // Close file
  s.close();
}

} // END report NAMESPACE
