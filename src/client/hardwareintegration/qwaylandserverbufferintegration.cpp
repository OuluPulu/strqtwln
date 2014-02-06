#include "qwaylandserverbufferintegration_p.h"

QT_BEGIN_NAMESPACE

QWaylandServerBuffer::QWaylandServerBuffer()
    : m_user_data(0)
{
}

QWaylandServerBuffer::~QWaylandServerBuffer()
{
}

QWaylandServerBuffer::Format QWaylandServerBuffer::format() const
{
    return m_format;
}

QSize QWaylandServerBuffer::size() const
{
    return m_size;
}

void QWaylandServerBuffer::setUserData(void *userData)
{
    m_user_data = userData;
}

void *QWaylandServerBuffer::userData() const
{
    return m_user_data;
}

QWaylandServerBufferIntegration::QWaylandServerBufferIntegration()
{
}
QWaylandServerBufferIntegration::~QWaylandServerBufferIntegration()
{
}

QT_END_NAMESPACE
