#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstbc63xclock.h"

#ifdef G_OS_WIN32
#  define WIN32_LEAN_AND_MEAN   /* prevents from including too many things */
#  include <windows.h>          /* Need for BC637PCI.h on Windows */
#  undef WIN32_LEAN_AND_MEAN
#  ifndef EWOULDBLOCK
#  define EWOULDBLOCK EAGAIN    /* This is just to placate gcc */
#  endif
#endif /* G_OS_WIN32 */
#include "BC637PCI.h"

enum
{
  PROP_0,
  PROP_DEVICE_NUMBER
};

static gboolean bc63xOpened = FALSE;


struct _GstBc63xClockPrivate
{
  guint device_number;
  GstClock *bc_clock;
};


#define gst_bc63x_clock_parent_class parent_class
G_DEFINE_TYPE_WITH_PRIVATE (GstBc63xClock, gst_bc63x_clock, GST_TYPE_SYSTEM_CLOCK);

static void gst_bc63x_clock_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_bc63x_clock_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);
static void gst_bc63x_clock_finalize (GObject * object);
static GstClockTime gst_bc63x_clock_get_internal_time (GstClock * clock);


static void
gst_bc63x_clock_class_init (GstBc63xClockClass * klass)
{
  GObjectClass *gobject_class;
  GstClockClass *clock_class;

  gobject_class = G_OBJECT_CLASS (klass);
  clock_class = GST_CLOCK_CLASS (klass);

  gobject_class->finalize = gst_bc63x_clock_finalize;
  gobject_class->get_property = gst_bc63x_clock_get_property;
  gobject_class->set_property = gst_bc63x_clock_set_property;

  g_object_class_install_property (gobject_class, PROP_DEVICE_NUMBER,
      g_param_spec_uint ("device-number", "Device number",
          "Timing card instance to use", 0, G_MAXUINT, 0,
          (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
              G_PARAM_CONSTRUCT)));

  clock_class->get_internal_time = gst_bc63x_clock_get_internal_time;
  //clock_class->get_resolution = GST_NSECOND * G_GINT64_CONSTANT(100);

}

static void
gst_bc63x_clock_init (GstBc63xClock * self)
{
  //self->priv->device_number = 0;
  GstBc63xClockPrivate *priv;

  self->priv = priv = gst_bc63x_clock_get_instance_private (self);


}

static void
gst_bc63x_clock_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstBc63xClock *self = GST_BC63X_CLOCK(object);

  switch (prop_id) {
    case PROP_DEVICE_NUMBER:
      self->priv->device_number = g_value_get_uint (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;

  }
}

static void
gst_bc63x_clock_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstBc63xClock *self = GST_BC63X_CLOCK(object);

  switch (prop_id) {
    case PROP_DEVICE_NUMBER:
      g_value_set_uint (value, self->priv->device_number);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_bc63x_clock_finalize (GObject * object)
{
  GstBc63xClock *self = GST_BC63X_CLOCK (object);

  // if (self->priv->domain_stats_id)
  //   gst_ptp_statistics_callback_remove (self->priv->domain_stats_id);

  G_OBJECT_CLASS (gst_bc63x_clock_parent_class)->finalize (object);
}

static GstClockTime
gst_bc63x_clock_get_internal_time (GstClock * clock)
{
  GstBc63xClock *self;
  GstClockTime result;

  self = GST_BC63X_CLOCK (clock);

  if (!bc63xOpened) {
    GST_ERROR_OBJECT (self, "bc63xPCIe is not opened");
    return GST_CLOCK_TIME_NONE;
  }   

  unsigned long maj, min; // seconds, microseconds
  unsigned short nano; // 100s of nanoseconds
  unsigned char pstat; // status bits

  gint64 timestamp;

  if (bcReadBinTimeEx(&maj, &min, &nano, &pstat) != RC_OK)
  {
    GST_ERROR_OBJECT (self, "bc63xPCIe unable to read time");
    return GST_CLOCK_TIME_NONE;
      
  }
  timestamp = (maj * GST_SECOND) + (min * GST_MSECOND) + (nano * G_GINT64_CONSTANT(100) * GST_NSECOND);

  return timestamp;
}

gboolean
gst_bc63x_is_initialized (void)
{
  return bc63xOpened;
}


gboolean
gst_bc63x_init (guint64 clock_id, guint deviceIndex)
{
  gboolean ret = FALSE;

  if(bcStartPCI(deviceIndex) == RC_OK)
  {
    bc63xOpened = TRUE;
    ret = TRUE;
    
  }


  return ret;
}


GstClock *
gst_bc63x_clock_new (const gchar * name, guint deviceIndex)
{
  GstClock *clock;

  g_return_val_if_fail (deviceIndex <= G_MAXUINT8, NULL);

  if (!bc63xOpened && !gst_bc63x_init (GST_BC63X_CLOCK_ID_NONE, deviceIndex)) {
    GST_ERROR ("Failed to initialize BC63X");
    return NULL;
  }

  clock = g_object_new (GST_TYPE_BC63X_CLOCK, "name", name, "device-number", deviceIndex,
      NULL);

  /* Clear floating flag */
  gst_object_ref_sink (clock);

  return clock;
}
