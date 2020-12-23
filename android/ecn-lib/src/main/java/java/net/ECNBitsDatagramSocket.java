package java.net;

/*-
 * Copyright © 2020
 *      mirabilos <t.glaser@tarent.de>
 * Licensor: Deutsche Telekom
 *
 * Provided that these terms and disclaimer and all copyright notices
 * are retained or reproduced in an accompanying document, permission
 * is granted to deal in this work without restriction, including un‐
 * limited rights to use, publicly perform, distribute, sell, modify,
 * merge, give away, or sublicence.
 *
 * This work is provided “AS IS” and WITHOUT WARRANTY of any kind, to
 * the utmost extent permitted by applicable law, neither express nor
 * implied; without malicious intent or gross negligence. In no event
 * may a licensor, author or contributor be held liable for indirect,
 * direct, other damage, loss, or other issues arising in any way out
 * of dealing in the work, even if advised of the possibility of such
 * damage or existence of a defect, except proven that it results out
 * of said person’s immediate fault when using the work as intended.
 */

import android.util.Log;
import de.telekom.llcto.ecn_bits.android.lib.AbstractECNBitsDatagramReceiver;
import de.telekom.llcto.ecn_bits.android.lib.AbstractECNBitsDatagramSocket;
import de.telekom.llcto.ecn_bits.android.lib.ECNBitsLibraryException;
import de.telekom.llcto.ecn_bits.android.lib.ECNMeasurer;
import de.telekom.llcto.ecn_bits.android.lib.ECNStatistics;

import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

/**
 * {@link DatagramSocket} equivalent capable of returning and
 * collecting ECN bits and the traffic class octet from received packets.
 * Not suitable for use with IP Multicast.
 *
 * This class offers the methods from {@link AbstractECNBitsDatagramReceiver}
 * to determine the IP traffic class and thus the ECN bits.
 *
 * @author mirabilos (t.glaser@tarent.de)
 */
public class ECNBitsDatagramSocket extends AbstractECNBitsDatagramSocket {
    static {
        try {
            DatagramSocket.setDatagramSocketImplFactory(new ECNBitsDatagramSocketImplFactory());
        } catch (IOException e) {
            Log.e("ECN-Bits", "unable to setDatagramSocketImplFactory", e);
        }
    }

    private ECNBitsDatagramSocketImpl eimpl;
    private final ECNMeasurer tcm;

    /**
     * Constructs an ECN-capable datagram socket, wildcard bound to a free port
     *
     * @throws SocketException         on open or bind failures
     * @throws ECNBitsLibraryException if the ECN impl is not available
     * @see DatagramSocket#DatagramSocket()
     */
    public ECNBitsDatagramSocket() throws SocketException {
        this(new InetSocketAddress(0));
    }

    /**
     * Constructs an ECN-capable datagram socket, bound to bindaddr
     *
     * @param bindaddr the local socket address to bind to, null to not bind
     * @throws SocketException         on open or bind failures
     * @throws ECNBitsLibraryException if the ECN impl is not available
     * @see DatagramSocket#DatagramSocket(SocketAddress)
     */
    public ECNBitsDatagramSocket(final SocketAddress bindaddr) throws SocketException {
        super(bindaddr);

        final DatagramSocketImpl impl;
        try {
            final Method getImplRM = this.getClass().getSuperclass().getDeclaredMethod("getImpl");
            getImplRM.setAccessible(true);
            impl = (DatagramSocketImpl) getImplRM.invoke(this);
        } catch (IllegalAccessException | InvocationTargetException | NoSuchMethodException e) {
            close();
            throw new ECNBitsLibraryException("cannot get DatagramSocketImpl", e);
        }
        if (impl instanceof ECNBitsDatagramSocketImpl) {
            eimpl = (ECNBitsDatagramSocketImpl) impl;
            tcm = eimpl.setUpRecvTclass();
            return;
        }
        close();
        throw new ECNBitsLibraryException("not an ECNBitsDatagramSocketImpl");
    }

    /**
     * Constructs an ECN-capable datagram socket, wildcard bound to port
     *
     * @param port local port to bind to
     * @throws SocketException         on open or bind failures
     * @throws ECNBitsLibraryException if the ECN impl is not available
     * @see DatagramSocket#DatagramSocket(int)
     */
    @SuppressWarnings({ "unused", /* UnIntelliJ bug */ "RedundantSuppression" })
    public ECNBitsDatagramSocket(final int port) throws SocketException {
        this(port, null);
    }

    /**
     * Constructs an ECN-capable datagram socket, bound to laddr:port
     *
     * @param laddr local address to bind to
     * @param port  local port to bind to
     * @throws SocketException         on open or bind failures
     * @throws ECNBitsLibraryException if the ECN impl is not available
     * @see DatagramSocket#DatagramSocket(int, InetAddress)
     */
    public ECNBitsDatagramSocket(final int port, final InetAddress laddr) throws SocketException {
        this(new InetSocketAddress(laddr, port));
    }

    /**
     * Closes this datagram socket.
     *
     * Any thread currently blocked in {@link #receive} upon this socket
     * will throw a {@link SocketException}.
     */
    @Override
    public void close() {
        // remove strong reference
        eimpl = null;
        super.close();
    }

    @Override
    public Byte retrieveLastTrafficClass() {
        return tcm.last();
    }

    @Override
    public void startMeasurement() {
        tcm.doMeasuring(true, false);
    }

    @Override
    public ECNStatistics getMeasurement(final boolean doContinue) {
        return tcm.doMeasuring(doContinue, true);
    }
}
