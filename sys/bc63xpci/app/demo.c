#include <gst/gst.h>

#include "sys/bc63xpci/gstbc63xclock.h"

int
main (int argc, char *argv[])
{
  GstElement *pipeline, *source, *timecodestamper, *timeoverlay, *sink;
  GstBus *bus;
  GstMessage *msg;
  GstStateChangeReturn ret;
  GstElement* videofilter;
  


  /* Initialize GStreamer */
  gst_init (&argc, &argv);

  /* Create the elements */
  source = gst_element_factory_make ("videotestsrc", "source");
  sink = gst_element_factory_make ("autovideosink", "sink");
  timeoverlay = gst_element_factory_make ("timeoverlay", "timeoverlay");
  timecodestamper = gst_element_factory_make("timecodestamper", "timecodestamper");
  videofilter = gst_element_factory_make("capsfilter", NULL);

  
  


  /* Create the empty pipeline */
  pipeline = gst_pipeline_new ("test-pipeline");

  GstClock* clk = gst_bc63x_clock_new("bc635-clock", 0);
  gst_clock_wait_for_sync(clk, GST_CLOCK_TIME_NONE);
  gst_pipeline_use_clock(GST_PIPELINE(pipeline), clk);


  if (!pipeline || !source || !sink || !timecodestamper || !timeoverlay) {
    g_printerr ("Not all elements could be created.\n");
    return -1;
  }


  /* Build the pipeline */
  gst_bin_add_many (GST_BIN (pipeline), source, videofilter, timecodestamper, timeoverlay, sink, NULL);
  if (gst_element_link_many (source, videofilter, timecodestamper, timeoverlay, sink, NULL) != TRUE) {
    g_printerr ("Elements could not be linked.\n");
    gst_object_unref (pipeline);
    return -1;
  }


  /* Modify the source's properties */
  g_object_set (source, "pattern", 0, NULL);

  g_object_set(timecodestamper, "source", 5, NULL);
  g_object_set(timeoverlay, "time-mode", 3, NULL);

  GstCaps* videoCap;
  videoCap = gst_caps_from_string("video/x-raw,width=1280,height=720,framerate=100/1");
  g_object_set(G_OBJECT(videofilter), "caps", videoCap, NULL);
  gst_caps_unref(videoCap);
  

  /* Start playing */
  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr ("Unable to set the pipeline to the playing state.\n");
    gst_object_unref (pipeline);
    return -1;
  }

  //gst_pipeline_set_clock(GST_PIPELINE(pipeline), bcClock);

  /* Wait until error or EOS */
  bus = gst_element_get_bus (pipeline);
  msg =
      gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
      GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

  /* Parse message */
  if (msg != NULL) {
    GError *err;
    gchar *debug_info;

    switch (GST_MESSAGE_TYPE (msg)) {
      case GST_MESSAGE_ERROR:
        gst_message_parse_error (msg, &err, &debug_info);
        g_printerr ("Error received from element %s: %s\n",
            GST_OBJECT_NAME (msg->src), err->message);
        g_printerr ("Debugging information: %s\n",
            debug_info ? debug_info : "none");
        g_clear_error (&err);
        g_free (debug_info);
        break;
      case GST_MESSAGE_EOS:
        g_print ("End-Of-Stream reached.\n");
        break;
      default:
        /* We should not reach here because we only asked for ERRORs and EOS */
        g_printerr ("Unexpected message received.\n");
        break;
    }
    gst_message_unref (msg);
  }

  /* Free resources */
  gst_object_unref (bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  return 0;
}

