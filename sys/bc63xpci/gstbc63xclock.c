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
  GstClock *domain_clock;
};


#define gst_bc63x_clock_parent_class parent_class
G_DEFINE_TYPE_WITH_PRIVATE (GstBc63xClock, gst_bc63x_clock, GST_TYPE_SYSTEM_CLOCK);

static void gst_bc63x_clock_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_bc63x_clock_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);
static void gst_bc63x_clock_finalize (GObject * object);



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

  //clock_class->get_internal_time = gst_ptp_clock_get_internal_time;

}

static void
gst_bc63x_clock_init (GstBc63xClock * self)
{

}

static void
gst_bc63x_clock_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstBc63xClock *self = GST_BC63X_CLOCK(object);

  switch (prop_id) {
    case PROP_DEVICE_NUMBER:
      self->priv->device_number = g_value_get_int (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;

  }
}

static void
gst_bc63x_clock_finalize (GObject * object)
{

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

gboolean
gst_bc63x_is_initialized (void)
{
  return bc63xOpened;
}


gboolean
gst_bc63x_init (guint64 clock_id, gchar ** interfaces)
{

    return 1;
}


GstClock *
gst_bc63x_clock_new (const gchar * name, guint deviceIndex)
{
  GstClock *clock;

  g_return_val_if_fail (deviceIndex <= G_MAXUINT8, NULL);

  if (!bc63xOpened && !gst_bc63x_init (GST_BC63X_CLOCK_ID_NONE, NULL)) {
    GST_ERROR ("Failed to initialize BC63X");
    return NULL;
  }

  clock = g_object_new (GST_TYPE_BC63X_CLOCK, "name", name, "deviceIndex", deviceIndex,
      NULL);

  /* Clear floating flag */
  gst_object_ref_sink (clock);

  return clock;
}
