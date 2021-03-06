#include <pcl/surface/on_nurbs/fitting_curve_pdm.h>
#include <pcl/surface/on_nurbs/triangulation.h>

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>

#include <pcl/visualization/pcl_visualizer.h>

pcl::visualization::PCLVisualizer viewer ("Curve Fitting 3D");

void
PointCloud2Vector2d (pcl::PointCloud<pcl::PointXYZ>::Ptr cloud, pcl::on_nurbs::vector_vec3d &data)
{
  for (unsigned i = 0; i < cloud->size (); i++)
  {
    pcl::PointXYZ &p = cloud->at (i);
    if (!pcl_isnan (p.x) && !pcl_isnan (p.y) && !pcl_isnan (p.z))
      data.push_back (Eigen::Vector3d (p.x, p.y, p.z));
  }
}

void
VisualizeCurve (ON_NurbsCurve &curve, double r, double g, double b, bool show_cps)
{
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZRGB>);
  pcl::on_nurbs::Triangulation::convertCurve2PointCloud (curve, cloud, 8);

  for (std::size_t i = 0; i < cloud->size () - 1; i++)
  {
    pcl::PointXYZRGB &p1 = cloud->at (i);
    pcl::PointXYZRGB &p2 = cloud->at (i + 1);
    std::ostringstream os;
    os << "line_" << r << "_" << g << "_" << b << "_" << i;
    viewer.addLine<pcl::PointXYZRGB> (p1, p2, r, g, b, os.str ());
  }

  if (show_cps)
  {
    pcl::PointCloud<pcl::PointXYZ>::Ptr cps (new pcl::PointCloud<pcl::PointXYZ>);
    for (int i = 0; i < curve.CVCount (); i++)
    {
      ON_3dPoint cp;
      curve.GetCV (i, cp);

      pcl::PointXYZ p;
      p.x = float (cp.x);
      p.y = float (cp.y);
      p.z = float (cp.z);
      cps->push_back (p);
    }
    pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> handler (cps, 255 * r, 255 * g, 255 * b);
    viewer.addPointCloud<pcl::PointXYZ> (cps, handler, "cloud_cps");
  }
}

int
main (int argc, char *argv[])
{
  std::string pcd_file;

  if (argc > 1)
  {
    pcd_file = argv[1];
  }
  else
  {
    printf ("\nUsage: pcl_example_nurbs_fitting_curve3d pcd-file \n\n");
    printf ("  pcd-file    point-cloud file\n");
    exit (0);
  }

  // #################### LOAD FILE #########################
  printf ("  loading %s\n", pcd_file.c_str ());
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);
  pcl::PCLPointCloud2 cloud2;

  if (pcl::io::loadPCDFile (pcd_file, cloud2) == -1)
    throw std::runtime_error ("  PCD file not found.");

  fromPCLPointCloud2 (cloud2, *cloud);

  // convert to NURBS data structure
  pcl::on_nurbs::NurbsDataCurve data;
  PointCloud2Vector2d (cloud, data.interior);

  viewer.setSize (800, 600);
  viewer.addPointCloud<pcl::PointXYZ> (cloud, "cloud");

  // #################### CURVE PARAMETERS #########################
  unsigned order (3);
  unsigned n_control_points (20);

  pcl::on_nurbs::FittingCurve::Parameter curve_params;
  curve_params.smoothness = 0.000001;

  // #################### CURVE FITTING #########################
  ON_NurbsCurve curve = pcl::on_nurbs::FittingCurve::initNurbsCurvePCA (order, data.interior, n_control_points);

  pcl::on_nurbs::FittingCurve fit (&data, curve);
  fit.assemble (curve_params);
  fit.solve ();

  // visualize
  VisualizeCurve (fit.m_nurbs, 1.0, 0.0, 0.0, false);
  viewer.spin ();

  return 0;
}

