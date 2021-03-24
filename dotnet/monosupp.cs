using System;
using System.Net.Sockets;
using System.Runtime.InteropServices;

namespace ECNBits.DotNet.Experiment {

public class MonoSocketException : SocketException {
	#region IsRunningOnMono
	private static readonly Lazy<bool> IsThisMono = new Lazy<bool>(() => {
		return Type.GetType("Mono.Runtime") != null;
	});

	public static bool IsRunningOnMono() {
		return IsThisMono.Value;
	}
	#endregion

	#region native
	[DllImport(Unmanaged.LIB, CallingConvention=CallingConvention.Cdecl)]
	internal static extern int monosupp_errnomap(int errno);
	#endregion

	#region factory
	internal static SocketException NewSocketException() {
		if (!IsRunningOnMono())
			return new SocketException();
		int errno = Marshal.GetLastWin32Error();
		int winerr = monosupp_errnomap(errno);
		return new MonoSocketException(errno, winerr);
	}
	#endregion

	#region implementation
	internal MonoSocketException(int eno, int errorCode) : base(errorCode) {
		ErrnoValue = eno;
	}

	private int ErrnoValue;

	// this is unfortunately the only one overridable
	public override int ErrorCode => ErrnoValue;

	// ok SOL… both NativeErrorCode and SocketErrorCode are
	// not overridable, yet both return NativeErrorCode… and
	// that’s *precisely* which we *must* distinguish ☹

	public /*override*/new int NativeErrorCode {
		get {
			return ErrnoValue;
		}
	}
	#endregion
}

}
