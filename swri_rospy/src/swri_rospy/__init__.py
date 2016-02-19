import rospy
from rospy.impl.tcpros_base import DEFAULT_BUFF_SIZE
from Queue import Queue, Empty as EmptyQueueException
from threading import Condition

# The global callback queue
callback_queue = Queue()

def spin():
    '''
    This spinner is used in place of the rospy.spin() method. Whereas
    rospy.spin() simply waits for rospy.is_shutdown(), this spinner processes
    the callback queue until rospy.is_shutdown() is true. The callback
    queue is thread-safe, so for a multi-threaded queue processor, call this
    spin method in multiple threads.
    '''
    while not rospy.is_shutdown():
        try:
            condition = callback_queue.get(block=True, timeout=0.1)
        except EmptyQueueException:
            pass
        else:
            with condition:
                condition.notify()
                condition.wait()

def single_threaded(callback):
    '''
    This decorator can be used on service or subscriber callbacks. A callback
    wrapped with this decorator will put a condition on the callback_queue and
    then wait its turn before executing the decorated callback.
    '''
    # Prevent recursive decoration, which would deadlock
    if hasattr(callback, 'single_threaded') and callback.single_threaded:
        return callback

    def wrapped_callback(*args, **kwds):
        # Put condition on queue
        condition = Condition()
        with condition:
            callback_queue.put(condition)
            # Wait for condition
            condition.wait()
            # Do the callback
            try:
                ret = callback(*args, **kwds)
            finally:
                condition.notify()
            return ret
    wrapped_callback.single_threaded = True
    return wrapped_callback

def service_wrapper(func):
    '''
    This is a decorator for functions that handle ROS service calls.
    It catches unhandled exceptions and reports them cleanly as errors
    on the ROS console.
    '''
    def wrapper(*args, **kwds):
        try:
            return func(*args, **kwds)
        except:
            rospy.logerr(traceback.format_exc())
            return None
    return wrapper


def subscribe_single_threaded(name, data_class, callback=None,
                              callback_args=None, queue_size=None,
                              buff_size=DEFAULT_BUFF_SIZE, tcp_nodelay=False):
    '''Create a Subscriber with single-threaded callback.

    All parameter definitions are the same as for rospy.Subscriber. Callback
    will be wrapped with single_threaded decorator above.

    Returns a rospy.Subscriber defined exactly as if it had been called
    directly, but with a single-threaded callback.
    '''

    # rospy.SubscriberImpl._invoke_callback calls the callback with different
    # arities depending on callback_args, so we have to supply a function with
    # the expected signature
    if callback_args is None:
        @single_threaded
        def single_threaded_callback(msg):
            callback(msg)
    else:
        @single_threaded
        def single_threaded_callback(msg, args):
            callback(msg, args)

    # All arguments except the callback are passed straight through
    return rospy.Subscriber(name, data_class, single_threaded_callback,
                            callback_args, queue_size, buff_size, tcp_nodelay)
