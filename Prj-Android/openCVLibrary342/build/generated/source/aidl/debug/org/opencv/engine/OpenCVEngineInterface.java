/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Original file: /Users/mac02/Desktop/otherspace/githubProject/HyperLPR/Prj-Android/openCVLibrary342/src/main/aidl/org/opencv/engine/OpenCVEngineInterface.aidl
 */
package org.opencv.engine;
/**
* Class provides a Java interface for OpenCV Engine Service. It's synchronous with native OpenCVEngine class.
*/
public interface OpenCVEngineInterface extends android.os.IInterface
{
/** Local-side IPC implementation stub class. */
public static abstract class Stub extends android.os.Binder implements org.opencv.engine.OpenCVEngineInterface
{
private static final java.lang.String DESCRIPTOR = "org.opencv.engine.OpenCVEngineInterface";
/** Construct the stub at attach it to the interface. */
public Stub()
{
this.attachInterface(this, DESCRIPTOR);
}
/**
 * Cast an IBinder object into an org.opencv.engine.OpenCVEngineInterface interface,
 * generating a proxy if needed.
 */
public static org.opencv.engine.OpenCVEngineInterface asInterface(android.os.IBinder obj)
{
if ((obj==null)) {
return null;
}
android.os.IInterface iin = obj.queryLocalInterface(DESCRIPTOR);
if (((iin!=null)&&(iin instanceof org.opencv.engine.OpenCVEngineInterface))) {
return ((org.opencv.engine.OpenCVEngineInterface)iin);
}
return new org.opencv.engine.OpenCVEngineInterface.Stub.Proxy(obj);
}
@Override public android.os.IBinder asBinder()
{
return this;
}
@Override public boolean onTransact(int code, android.os.Parcel data, android.os.Parcel reply, int flags) throws android.os.RemoteException
{
switch (code)
{
case INTERFACE_TRANSACTION:
{
reply.writeString(DESCRIPTOR);
return true;
}
case TRANSACTION_getEngineVersion:
{
data.enforceInterface(DESCRIPTOR);
int _result = this.getEngineVersion();
reply.writeNoException();
reply.writeInt(_result);
return true;
}
case TRANSACTION_getLibPathByVersion:
{
data.enforceInterface(DESCRIPTOR);
java.lang.String _arg0;
_arg0 = data.readString();
java.lang.String _result = this.getLibPathByVersion(_arg0);
reply.writeNoException();
reply.writeString(_result);
return true;
}
case TRANSACTION_installVersion:
{
data.enforceInterface(DESCRIPTOR);
java.lang.String _arg0;
_arg0 = data.readString();
boolean _result = this.installVersion(_arg0);
reply.writeNoException();
reply.writeInt(((_result)?(1):(0)));
return true;
}
case TRANSACTION_getLibraryList:
{
data.enforceInterface(DESCRIPTOR);
java.lang.String _arg0;
_arg0 = data.readString();
java.lang.String _result = this.getLibraryList(_arg0);
reply.writeNoException();
reply.writeString(_result);
return true;
}
}
return super.onTransact(code, data, reply, flags);
}
private static class Proxy implements org.opencv.engine.OpenCVEngineInterface
{
private android.os.IBinder mRemote;
Proxy(android.os.IBinder remote)
{
mRemote = remote;
}
@Override public android.os.IBinder asBinder()
{
return mRemote;
}
public java.lang.String getInterfaceDescriptor()
{
return DESCRIPTOR;
}
@Override public int getEngineVersion() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
int _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_getEngineVersion, _data, _reply, 0);
_reply.readException();
_result = _reply.readInt();
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public java.lang.String getLibPathByVersion(java.lang.String version) throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
java.lang.String _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
_data.writeString(version);
mRemote.transact(Stub.TRANSACTION_getLibPathByVersion, _data, _reply, 0);
_reply.readException();
_result = _reply.readString();
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
/**
    * Tries to install defined version of OpenCV from Google Play Market.
    * @param OpenCV version.
    * @return Returns true if installation was successful or OpenCV package has been already installed.
    */
@Override public boolean installVersion(java.lang.String version) throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
boolean _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
_data.writeString(version);
mRemote.transact(Stub.TRANSACTION_installVersion, _data, _reply, 0);
_reply.readException();
_result = (0!=_reply.readInt());
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public java.lang.String getLibraryList(java.lang.String version) throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
java.lang.String _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
_data.writeString(version);
mRemote.transact(Stub.TRANSACTION_getLibraryList, _data, _reply, 0);
_reply.readException();
_result = _reply.readString();
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
}
static final int TRANSACTION_getEngineVersion = (android.os.IBinder.FIRST_CALL_TRANSACTION + 0);
static final int TRANSACTION_getLibPathByVersion = (android.os.IBinder.FIRST_CALL_TRANSACTION + 1);
static final int TRANSACTION_installVersion = (android.os.IBinder.FIRST_CALL_TRANSACTION + 2);
static final int TRANSACTION_getLibraryList = (android.os.IBinder.FIRST_CALL_TRANSACTION + 3);
}
public int getEngineVersion() throws android.os.RemoteException;
public java.lang.String getLibPathByVersion(java.lang.String version) throws android.os.RemoteException;
/**
    * Tries to install defined version of OpenCV from Google Play Market.
    * @param OpenCV version.
    * @return Returns true if installation was successful or OpenCV package has been already installed.
    */
public boolean installVersion(java.lang.String version) throws android.os.RemoteException;
public java.lang.String getLibraryList(java.lang.String version) throws android.os.RemoteException;
}
