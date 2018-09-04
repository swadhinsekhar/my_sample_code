/*
	gst-launch-1.0 -v v4l2src device=/dev/video0 do-timestamp=true ! "video/x-raw, format=I420, width=640, height=480, framerate=30/1" ! queue ! x264enc ! h264parse ! rtph264pay config-interval=1 ! udpsink host=192.168.4.181 port=5001 

	//Debug log enable	
	export GST_DEBUG="*:6"	

	//To compile 
	gcc `pkg-config --cflags --libs glib-2.0` cameraudpsink.c -o cameraudpsink `pkg-config --cflags --libs gstreamer-1.0` -lglib-2.0 -levent
*/

#include <gst/gst.h>
#include <stdio.h>

#define PORT 5001
#define IP "192.168.4.181"

GstElement *pipeline;

gboolean internal_bus_call (GstBus *bus, GstMessage *msg, gpointer data)
{
  GMainLoop *loop = (GMainLoop *) data;

  switch (GST_MESSAGE_TYPE (msg)) {
  	case GST_MESSAGE_EOS:
    	g_print ("From internal bus! End of stream\n");
      g_main_loop_quit (loop);
      break;

    case GST_MESSAGE_ERROR: {
      gchar  *debug_info;
      GError *error;

      gst_message_parse_error (msg, &error, &debug_info);
      g_print("From internal bus! Error: %s\n", error->message);
      g_print("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), error->message);
      g_print("Debugging information: %s\n", debug_info ? debug_info : "none");
      g_free (debug_info);
      g_error_free (error);
      g_main_loop_quit (loop);
      break;
    }

		case GST_MESSAGE_WARNING: {
      const GstStructure *s;

      s = gst_message_get_structure (msg);
      g_print("WARNING! message from \"%s\" (%s)",
            GST_STR_NULL (GST_ELEMENT_NAME (GST_MESSAGE_SRC (msg))),
            gst_message_type_get_name (GST_MESSAGE_TYPE (msg)));
      if(s) {
        gchar *sstr;

        sstr = gst_structure_to_string (s);
        g_print ("%s\n", sstr);
        g_free (sstr);
      }
      break;
    }

    case GST_MESSAGE_STATE_CHANGED:
      /* We are only interested in state-changed messages from pipeline */
      if(GST_MESSAGE_SRC(msg) == GST_OBJECT(pipeline)) {
        GstState old_state, new_state, pending_state;
        gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);
        g_print("\nPipeline state changed from: %s to: %s\n",
        gst_element_state_get_name(old_state), gst_element_state_get_name(new_state));
        g_print("pending state %s\n\n",  gst_element_state_get_name(pending_state));
      }
      break;

    default:
      break;
  }

  return TRUE;
}

int main()
{
  GstElement *v4l2src, *capsfilter, *queue, *x264enc, *h264parse, *rtph264pay, *udpsink;
  GMainLoop *loop;
  GstBus *bus;
  GstMessage *msg;
  guint bus_watch_id;
  GstCaps *temp_caps =  NULL;
  gchar name[100];

  gst_init(0, NULL);
  loop = g_main_loop_new (NULL, FALSE);

  pipeline = gst_pipeline_new("my-pipeline");

	v4l2src = gst_element_factory_make ("v4l2src", "v4l2src");
	capsfilter = gst_element_factory_make ("capsfilter", "cfilter");
	queue = gst_element_factory_make ("queue", "queue");
	x264enc = gst_element_factory_make ("x264enc", "xenc");
	h264parse = gst_element_factory_make ("h264parse", "hparse");
	rtph264pay = gst_element_factory_make ("rtph264pay", "rtppay");
	udpsink = gst_element_factory_make ("udpsink", "usink");

	if(!pipeline || !v4l2src || !capsfilter 	|| !queue
			|| !x264enc || !h264parse 	|| !rtph264pay || !udpsink ) {
		g_printerr("Unable to create all elements.\n");
		gst_object_unref (pipeline);
	}

	gst_bin_add_many (GST_BIN(pipeline), v4l2src, capsfilter, 
					queue, x264enc, h264parse, rtph264pay, udpsink, NULL);

	if(!gst_element_link_many (v4l2src, capsfilter, queue, x264enc, h264parse, rtph264pay, udpsink, NULL)) {
		g_printerr("Unable to link all elements.\n");
		gst_object_unref(pipeline);
		return -1;
	} 
	
	g_object_set(G_OBJECT(v4l2src), "device", "/dev/video0", NULL); 
	g_object_set(G_OBJECT(v4l2src), "do-timestamp", TRUE, NULL);

	g_snprintf (name, 100, "video/x-raw, format=(string)I420, width=(int)%d, height=(int)%d, framerate=30/1", 640, 360);
	temp_caps = gst_caps_from_string (name);
	if(!temp_caps) {
		g_printerr("Unable to create capsfilter!");	
		return -1;
	}

	g_object_set(G_OBJECT(rtph264pay), "config-interval", 1, NULL);
	g_object_set(G_OBJECT(capsfilter), "caps", temp_caps, NULL);
	g_object_set(G_OBJECT(udpsink), "port", PORT, NULL); 
	g_object_set(G_OBJECT(udpsink), "host", IP, NULL);
	g_object_set(G_OBJECT(udpsink), "sync", TRUE, NULL); 

  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  bus_watch_id = gst_bus_add_watch (bus, internal_bus_call, loop);
  gst_object_unref (bus);

  g_print ("Now playing ...\n");
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

  g_print ("Running...\n");
  g_main_loop_run (loop);
	
  g_print ("Returned and stopping playback...\n");
  gst_element_set_state (pipeline, GST_STATE_NULL);
  g_print ("Deleting pipeline...\n");
  gst_object_unref (GST_OBJECT(pipeline));
	g_source_remove (bus_watch_id);
	g_main_loop_unref (loop);

	return 0;
}
