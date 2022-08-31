#ifndef __GST_BC63X_CLOCK_H__
#define __GST_BC63X_CLOCK_H__

#include <gst/gst.h>
#include <gst/gstsystemclock.h>

G_BEGIN_DECLS

#define GST_TYPE_BC63X_CLOCK \
  (gst_bc63x_clock_get_type())
#define GST_BC63X_CLOCK(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_BC63X_CLOCK,GstBc63xClock))
#define GST_BC63X_CLOCK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_BC63X_CLOCK,GstBc63xClockClass))
#define GST_IS_BC63X_CLOCK(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_BC63X_CLOCK))
#define GST_IS_BC63X_CLOCK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_BC63X_CLOCK))


typedef struct _GstBc63xClock GstBc63xClock;
typedef struct _GstBc63xClockClass GstBc63xClockClass;
typedef struct _GstBc63xClockPrivate GstBc63xClockPrivate;

/**
 * GstBc63xClock:
 *
 * Opaque #GstBc63xClock structure.
 */
struct _GstBc63xClock {
  GstSystemClock clock;

  /*< private >*/
  GstBc63xClockPrivate *priv;
  

  gpointer _gst_reserved[GST_PADDING];
};

/**
 * GstBc63xClockClass:
 * @parent_class: parented to #GstSystemClockClass
 *
 * Opaque #GstBc63xClockClass structure.
 */
struct _GstBc63xClockClass {
  GstSystemClockClass parent_class;

  /*< private >*/
  gpointer _gst_reserved[GST_PADDING];
};

/**
 * GST_BC63X_CLOCK_ID_NONE:
 * BC63X clock identification that can be passed to gst_bc63x_init() to
 * automatically select one based on the MAC address of interfaces
 */
#define GST_BC63X_CLOCK_ID_NONE ((guint64) -1)

GST_API
GType           gst_bc63x_clock_get_type             (void);

GST_API
gboolean        gst_bc63x_is_supported               (void);

GST_API
gboolean        gst_bc63x_is_initialized             (void);

GST_API
gboolean        gst_bc63x_init                       (guint64 clock_id,
                                                    guint deviceIndex);
GST_API
void            gst_bc63x_deinit                     (void);

GST_API
GstClock*       gst_bc63x_clock_new                  (const gchar *name,
                                                    guint deviceIndex);


G_DEFINE_AUTOPTR_CLEANUP_FUNC(GstBc63xClock, gst_object_unref)

G_END_DECLS

#endif /* __GST_BC63X_CLOCK_H__ */