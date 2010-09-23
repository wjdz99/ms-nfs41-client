/* Copyright (c) 2010
 * The Regents of the University of Michigan
 * All Rights Reserved
 *
 * Permission is granted to use, copy and redistribute this software
 * for noncommercial education and research purposes, so long as no
 * fee is charged, and so long as the name of the University of Michigan
 * is not used in any advertising or publicity pertaining to the use
 * or distribution of this software without specific, written prior
 * authorization.  Permission to modify or otherwise create derivative
 * works of this software is not granted.
 *
 * This software is provided as is, without representation or warranty
 * of any kind either express or implied, including without limitation
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or noninfringement.  The Regents of the University of
 * Michigan shall not be liable for any damages, including special,
 * indirect, incidental, or consequential damages, with respect to any
 * claim arising out of or in connection with the use of the software,
 * even if it has been or is hereafter advised of the possibility of
 * such damages.
 */

#define MINIRDR__NAME "Value is ignored, only fact of definition"
#include <rx.h>

#include "nfs41_driver.h"
#include "nfs41_debug.h"
#include <stdio.h>
#include <stdarg.h>
#include <ntstrsafe.h>
#include <winerror.h>

//#define INCLUDE_TIMESTAMPS

ULONG __cdecl DbgP(IN PCCH fmt, ...)
{
    CHAR msg[512];
    va_list args;
    NTSTATUS status;

    va_start(args, fmt);
    ASSERT(fmt != NULL);
    status = RtlStringCbVPrintfA(msg, sizeof(msg), fmt, args);
    if (NT_SUCCESS(status)) {
#ifdef INCLUDE_TIMESTAMPS
        LARGE_INTEGER timestamp, local_time;
        TIME_FIELDS time_fields;

        KeQuerySystemTime(&timestamp);
        ExSystemTimeToLocalTime(&timestamp,&local_time);
        RtlTimeToTimeFields(&local_time, &time_fields);

        DbgPrintEx(DPFLTR_IHVNETWORK_ID, DPFLTR_ERROR_LEVEL, 
            "[%ld].[%02u:%02u:%02u.%u] %s", IoGetCurrentProcess(), 
            time_fields.Hour, time_fields.Minute, time_fields.Second, 
            time_fields.Milliseconds, msg);
#else
        DbgPrintEx(DPFLTR_IHVNETWORK_ID, DPFLTR_ERROR_LEVEL, 
            "[%ld] %s", IoGetCurrentProcess(), msg);
#endif
    }
    va_end(args);

    return 0;
}

void print_hexbuf(int on, unsigned char *title, unsigned char *buf, int len) 
{
    int j, k;
    LARGE_INTEGER timestamp, local_time;
    TIME_FIELDS time_fields;

    if (!on) return;

    KeQuerySystemTime(&timestamp);
    ExSystemTimeToLocalTime(&timestamp,&local_time);
    RtlTimeToTimeFields(&local_time, &time_fields);

    DbgPrintEx(DPFLTR_IHVNETWORK_ID, DPFLTR_ERROR_LEVEL, 
        "[%ld].[%02u:%02u:%02u.%u] %s\n", IoGetCurrentProcess(), 
        time_fields.Hour, time_fields.Minute, time_fields.Second, 
        time_fields.Milliseconds, title);
    for(j = 0, k = 0; j < len; j++, k++) {
        DbgPrintEx(DPFLTR_IHVNETWORK_ID, DPFLTR_ERROR_LEVEL,
            "%02x ", buf[j]);
        if (((k+1) % 30 == 0 && k > 0))
            DbgPrintEx(DPFLTR_IHVNETWORK_ID, DPFLTR_ERROR_LEVEL, "\n");
    }
    DbgPrintEx(DPFLTR_IHVNETWORK_ID, DPFLTR_ERROR_LEVEL, "\n");
}

void print_ioctl(int on, int op)
{
    if(!on) return;
    switch(op) {
        case IRP_MJ_FILE_SYSTEM_CONTROL:
            DbgP("IRP_MJ_FILE_SYSTEM_CONTROL\n");
            break;
        case IRP_MJ_DEVICE_CONTROL:
            DbgP("IRP_MJ_DEVICE_CONTROL\n");
            break;
        case IRP_MJ_INTERNAL_DEVICE_CONTROL:
            DbgP("IRP_MJ_INTERNAL_DEVICE_CONTROL\n");
            break;
        default:
            DbgP("UNKNOWN MJ IRP %d\n", op);
    };
}

void print_fs_ioctl(int on, int op)
{
    if(!on) return;
    switch(op) {
        case IOCTL_NFS41_INVALCACHE:
            DbgP("IOCTL_NFS41_INVALCACHE\n");
            break;
        case IOCTL_NFS41_READ:
            DbgP("IOCTL_NFS41_UPCALL\n");
            break;
        case IOCTL_NFS41_WRITE:
            DbgP("IOCTL_NFS41_DOWNCALL\n");
            break;
        case IOCTL_NFS41_ADDCONN:
            DbgP("IOCTL_NFS41_ADDCONN\n");
            break;
        case IOCTL_NFS41_DELCONN:
            DbgP("IOCTL_NFS41_DELCONN\n");
            break;
        case IOCTL_NFS41_GETSTATE:
            DbgP("IOCTL_NFS41_GETSTATE\n");
            break;
        case IOCTL_NFS41_START:
            DbgP("IOCTL_NFS41_START\n");
            break;
        case IOCTL_NFS41_STOP:
            DbgP("IOCTL_NFS41_STOP\n");
            break;
        default:
            DbgP("UNKNOWN FS IOCTL %d\n", op);
    };
}

void print_driver_state(int state)
{
    switch (state) {
        case NFS41_START_DRIVER_STARTABLE:
            DbgP("NFS41_START_DRIVER_STARTABLE\n");
            break;
        case NFS41_START_DRIVER_STOPPED:
            DbgP("NFS41_START_DRIVER_STOPPED\n");
            break;
        case NFS41_START_DRIVER_START_IN_PROGRESS:
            DbgP("NFS41_START_DRIVER_START_IN_PROGRESS\n");
            break;
        case NFS41_START_DRIVER_STARTED:
            DbgP("NFS41_START_DRIVER_STARTED\n");
            break;
        default:
            DbgP("UNKNOWN DRIVER STATE %d\n", state);
    };

}

void print_basic_info(int on, PFILE_BASIC_INFORMATION info)
{
    if (!on) return;
    DbgP("BASIC_INFO: Create=%x Access=%x Write=%x Change=%x Attr=%x\n",
        info->CreationTime.QuadPart, info->LastAccessTime.QuadPart,
        info->LastWriteTime.QuadPart, info->ChangeTime.QuadPart, 
        info->FileAttributes);
}
void print_std_info(int on, PFILE_STANDARD_INFORMATION info)
{
    if (!on) return;
    DbgP("STD_INFO: Type=%s #Links=%d Alloc=%x EOF=%x Delete=%d\n",
        info->Directory?"DIR":"FILE", info->NumberOfLinks, 
        info->AllocationSize, info->EndOfFile, info->DeletePending);
}

void print_ea_info(int on, PFILE_FULL_EA_INFORMATION info)
{
    if (!on) return;
    DbgP("FULL_EA_INFO: NextOffset=%d Flags=%x EaNameLength=%d "
        "ExValueLength=%x EaName=%s\n", info->NextEntryOffset, info->Flags,
        info->EaNameLength, info->EaValueLength, info->EaName);
    if (info->EaValueLength) 
        print_hexbuf(0, (unsigned char *)"eavalue", 
            (unsigned char *)info->EaName + info->EaNameLength + 1, 
            info->EaValueLength);
}

void print_get_ea(int on, PFILE_GET_EA_INFORMATION info)
{
    if (!on) return;
    DbgP("GET_EA_INFO: NextOffset=%d EaNameLength=%d EaName=%s\n", 
        info->NextEntryOffset, info->EaNameLength, info->EaName);
}

VOID print_srv_call(int on, IN PMRX_SRV_CALL p)
{
    if (!on) return;
    DbgP("PMRX_SRV_CALL %p\n", p);
#if 0
    DbgP("\tNodeReferenceCount %ld\n", p->NodeReferenceCount);
    //DbgP("Context %p\n", p->Context);
    //DbgP("Context2 %p\n", p->Context2);
    //DbgP("pSrvCallName %wZ\n", p->pSrvCallName);
    //DbgP("pPrincipalName %wZ\n", p->pPrincipalName);
    //DbgP("PDomainName %wZ\n", p->pDomainName);
    //DbgP("Flags %08lx\n", p->Flags);
    //DbgP("MaximumNumberOfCloseDelayedFiles %ld\n", p->MaximumNumberOfCloseDelayedFiles);
    //DbgP("Status %ld\n", p->Status);
    DbgP("*****************\n");
#endif
}

VOID print_net_root(int on, IN PMRX_NET_ROOT p)
{
    if (!on) return;
    DbgP("PMRX_NET_ROOT %p\n", p);
#if 0
    DbgP("\tNodeReferenceCount %ld\n", p->NodeReferenceCount);
    DbgP("\tpSrvCall %p\n", p->pSrvCall);
    //DbgP("Context %p\n", p->Context);
    //DbgP("Context2 %p\n", p->Context2);
    //DbgP("Flags %08lx\n", p->Flags);
    DbgP("\tNumberOfFcbs %ld\n", p->NumberOfFcbs);
    DbgP("\tNumberofSrvOpens %ld\n", p->NumberOfSrvOpens);
    //DbgP("MRxNetRootState %ld\n", p->MRxNetRootState);
    //DbgP("Type %ld\n", p->Type);
    //DbgP("DeviceType %ld\n", p->DeviceType);
    //DbgP("pNetRootName %wZ\n", p->pNetRootName);
    //DbgP("InnerNamePrefix %wZ\n", &p->InnerNamePrefix);
    DbgP("*****************\n");
#endif
}

VOID print_v_net_root(int on, IN PMRX_V_NET_ROOT p)
{
    if (!on) return;
    DbgP("PMRX_V_NET_ROOT %p\n", p);
#if 0
    DbgP("\tNodeReferenceCount %ld\n", p->NodeReferenceCount);
    DbgP("\tpNetRoot %p\n", p->pNetRoot);
    //DbgP("Context %p\n", p->Context);
    //DbgP("Context2 %p\n", p->Context2);
    //DbgP("Flags %08lx\n", p->Flags);
    DbgP("\tNumberofOpens %ld\n", p->NumberOfOpens);
    DbgP("\tNumberofFobxs %ld\n", p->NumberOfFobxs);
    //DbgP("LogonId\n");
    //DbgP("pUserDomainName %wZ\n", p->pUserDomainName);
    //DbgP("pUserName %wZ\n", p->pUserName);
    //DbgP("pPassword %wZ\n", p->pPassword);
    //DbgP("SessionId %ld\n", p->SessionId);
    //DbgP("ConstructionStatus %08lx\n", p->ConstructionStatus);
    //DbgP("IsExplicitConnection %d\n", p->IsExplicitConnection);
    DbgP("*****************\n");
#endif
}

void print_file_object(int on, PFILE_OBJECT file)
{
    if (!on) return;   
    DbgP("FsContext %p FsContext2 %p\n", file->FsContext, file->FsContext2);
    DbgP("DeletePending %d ReadAccess %d WriteAccess %d DeleteAccess %d\n",
        file->DeletePending, file->WriteAccess, file->DeleteAccess);
    DbgP("SharedRead %d SharedWrite %d SharedDelete %d Flags %x\n",
        file->SharedRead, file->SharedWrite, file->SharedDelete, file->Flags);
}

VOID print_fcb(int on, IN PMRX_FCB p)
{
    if (!on) return;
    DbgP("PMRX_FCB %p OpenCount %d\n", p, p->OpenCount);
#if 0
    DbgP("\tNodeReferenceCount %ld\n", p->NodeReferenceCount);
    DbgP("\tpNetRoot %p\n", p->pNetRoot);
    //DbgP("Context %p\n", p->Context);
    //DbgP("Context2 %p\n", p->Context2);
    //DbgP("FcbState %ld\n", p->FcbState);
    //DbgP("UncleanCount %ld\n", p->UncleanCount);
    //DbgP("UncachedUncleanCount %ld\n", p->UncachedUncleanCount);
    DbgP("\tOpenCount %ld\n", p->OpenCount);
    //DbgP("OutstandingLockOperationsCount %ld\n", p->OutstandingLockOperationsCount);
    //DbgP("ActualAllocationLength %ull\n", p->ActualAllocationLength);
    //DbgP("Attributes %ld\n", p->Attributes);
    //DbgP("IsFileWritten %d\n", p->IsFileWritten);
    //DbgP("fShouldBeOrphaned %d\n", p->fShouldBeOrphaned);
    //DbgP("fMiniInited %ld\n", p->fMiniInited);
    //DbgP("CachedNetRootType %c\n", p->CachedNetRootType);
    //DbgP("SrvOpenList\n");
    //DbgP("SrvOpenListVersion %ld\n", p->SrvOpenListVersion);
    DbgP("*****************\n");
#endif
}

VOID print_srv_open(int on, IN PMRX_SRV_OPEN p)
{
    if (!on) return;
    DbgP("PMRX_SRV_OPEN %p\n", p);
#if 0
    DbgP("\tNodeReferenceCount %ld\n", p->NodeReferenceCount);
    DbgP("\tpFcb %p\n", p->pFcb);
    DbgP("\tpVNetRoot %p\n", p->pVNetRoot);
    //DbgP("Context %p\n", p->Context);
    //DbgP("Context2 %p\n", p->Context2);
    //DbgP("Flags %08lx\n", p->Flags);
    //DbgP("pAlreadyPrefixedName %wZ\n", p->pAlreadyPrefixedName);
    //DbgP("UncleanFobxCount %ld\n", p->UncleanFobxCount);
    DbgP("\tOpenCount %ld\n", p->OpenCount);
    //DbgP("Key %p\n", p->Key);
    //DbgP("DesiredAccess\n");
    //DbgP("ShareAccess %ld\n", p->ShareAccess);
    //DbgP("CreateOptions %ld\n", p->CreateOptions);
    //DbgP("BufferingFlags %ld\n", p->BufferingFlags);
    //DbgP("ulFileSizeVersion %ld\n", p->ulFileSizeVersion);
    //DbgP("SrvOpenQLinks\n");
    DbgP("*****************\n");
#endif
}

VOID print_fobx(int on, IN PMRX_FOBX p)
{
    if (!on) return;
    DbgP("PMRX_FOBX %p\n", p);
#if 0
    DbgP("\tNodeReferenceCount %ld\n", p->NodeReferenceCount);
    DbgP("\tpSrvOpen %p\n", p->pSrvOpen);
    DbgP("\tAssociatedFileObject %p\n", p->AssociatedFileObject);
    //DbgP("Context %p\n", p->Context);
    //DbgP("Context2 %p\n", p->Context2);
    //DbgP("Flags %08lx\n", p->Flags);
    DbgP("*****************\n");
#endif
}

VOID print_irp_flags(int on, PIRP irp) 
{
    if (!on) return;
    DbgP("RxContext->CurrentIrp %p\n", irp);
    DbgP("IRP FLAGS:\n");
    if (irp->Flags & IRP_NOCACHE)
        DbgP("\tIRP_NOCACHE\n");
    if (irp->Flags & IRP_PAGING_IO)
        DbgP("\tIRP_PAGING_IO\n");
    if (irp->Flags & IRP_MOUNT_COMPLETION)
        DbgP("\tIRP_MOUNT_COMPLETION\n");
    if (irp->Flags & IRP_SYNCHRONOUS_API)
        DbgP("\tIRP_SYNCHRONOUS_API\n");
    if (irp->Flags & IRP_ASSOCIATED_IRP)
        DbgP("\tIRP_ASSOCIATED_IRP\n");
    if (irp->Flags & IRP_BUFFERED_IO)
        DbgP("\tIRP_BUFFERED_IO\n");
    if (irp->Flags & IRP_DEALLOCATE_BUFFER)
        DbgP("\tIRP_DEALLOCATE_BUFFER\n");
    if (irp->Flags & IRP_INPUT_OPERATION)
        DbgP("\tIRP_INPUT_OPERATION\n");
    if (irp->Flags & IRP_SYNCHRONOUS_PAGING_IO)
        DbgP("\tIRP_SYNCHRONOUS_PAGING_IO\n");
    if (irp->Flags & IRP_CREATE_OPERATION)
        DbgP("\tIRP_CREATE_OPERATION\n");
    if (irp->Flags & IRP_READ_OPERATION)
        DbgP("\tIRP_READ_OPERATION\n");
    if (irp->Flags & IRP_WRITE_OPERATION)
        DbgP("\tIRP_WRITE_OPERATION\n");
    if (irp->Flags & IRP_CLOSE_OPERATION)
        DbgP("\tIRP_CLOSE_OPERATION\n");
    if (irp->Flags & IRP_DEFER_IO_COMPLETION)
        DbgP("\tIRP_DEFER_IO_COMPLETION\n");
}

void print_irps_flags(int on, PIO_STACK_LOCATION irps)
{
    if (!on) return;
    DbgP("RxContext->CurrentIrpSp %p\n", irps);
    if(irps->Flags & SL_CASE_SENSITIVE)
        DbgP("\tSL_CASE_SENSITIVE\n");
    if(irps->Flags & SL_OPEN_PAGING_FILE)
        DbgP("\tSL_OPEN_PAGING_FILE\n");
    if(irps->Flags & SL_FORCE_ACCESS_CHECK)
        DbgP("\tSL_FORCE_ACCESS_CHECK\n");
    if(irps->Flags & SL_OPEN_TARGET_DIRECTORY)
        DbgP("\tSL_OPEN_TARGET_DIRECTORY\n");
}
void print_nt_create_params(int on, NT_CREATE_PARAMETERS params)
{
    if (!on) return;
    DbgP("File attributes %ld: %s %s %s %s %s\n", params.FileAttributes,
        (params.FileAttributes & FILE_ATTRIBUTE_TEMPORARY)?"TEMPFILE":"",
        (params.FileAttributes & FILE_ATTRIBUTE_READONLY)?"READONLY":"",
        (params.FileAttributes & FILE_ATTRIBUTE_HIDDEN)?"HIDDEN":"",
        (params.FileAttributes & FILE_ATTRIBUTE_SYSTEM)?"SYSTEM":"",
        (params.FileAttributes & FILE_ATTRIBUTE_ARCHIVE)?"ARCHIVE":"");

    if (params.Disposition  == FILE_SUPERSEDE)
        DbgP("Create Dispositions: FILE_SUPERSEDE\n");
    if (params.Disposition == FILE_CREATE)
        DbgP("Create Dispositions: FILE_CREATE\n");
    if (params.Disposition == FILE_OPEN)
        DbgP("Create Dispositions: FILE_OPEN\n");
    if (params.Disposition == FILE_OPEN_IF)
        DbgP("Create Dispositions: FILE_OPEN_IF\n");
    if (params.Disposition == FILE_OVERWRITE)
        DbgP("Create Dispositions: FILE_OVERWRITE\n");
    if (params.Disposition == FILE_OVERWRITE_IF)
        DbgP("Create Dispositions: FILE_OVERWRITE_IF\n");

    DbgP("Create Attributes: %s %s %s\n", 
        (params.CreateOptions & FILE_DIRECTORY_FILE)?"DIRFILE":"",
        (params.CreateOptions & FILE_NON_DIRECTORY_FILE)?"FILE":"",
        (params.CreateOptions & FILE_DELETE_ON_CLOSE)?"DELETEONCLOSE":"");
    if (on > 1) {
    DbgP("More Create Attrss:\n");
        if (params.CreateOptions & FILE_WRITE_THROUGH)
            DbgP("\tFILE_WRITE_THROUGH\n");
        if (params.CreateOptions & FILE_SEQUENTIAL_ONLY)
            DbgP("\tFILE_SEQUENTIAL_ONLY\n");
        if (params.CreateOptions & FILE_RANDOM_ACCESS)
            DbgP("\tFILE_RANDOM_ACCESS\n");
        if (params.CreateOptions & FILE_NO_INTERMEDIATE_BUFFERING)
            DbgP("\tFILE_NO_INTERMEDIATE_BUFFERING\n");
        if (params.CreateOptions & FILE_SYNCHRONOUS_IO_ALERT)
            DbgP("\tFILE_SYNCHRONOUS_IO_ALERT\n");
        if (params.CreateOptions & FILE_SYNCHRONOUS_IO_NONALERT)
            DbgP("\tFILE_SYNCHRONOUS_IO_NONALERT\n");
        if (params.CreateOptions & FILE_CREATE_TREE_CONNECTION)
            DbgP("\tFILE_CREATE_TREE_CONNECTION\n");
        if (params.CreateOptions & FILE_COMPLETE_IF_OPLOCKED)
            DbgP("\tFILE_COMPLETE_IF_OPLOCKED\n");
        if (params.CreateOptions & FILE_NO_EA_KNOWLEDGE)
            DbgP("\tFILE_NO_EA_KNOWLEDGE\n");
        if (params.CreateOptions & FILE_OPEN_REPARSE_POINT)
            DbgP("\tFILE_OPEN_REPARSE_POINT\n");
        if (params.CreateOptions & FILE_OPEN_BY_FILE_ID)
            DbgP("\tFILE_OPEN_BY_FILE_ID\n");
        if (params.CreateOptions & FILE_OPEN_FOR_BACKUP_INTENT)
            DbgP("\tFILE_OPEN_FOR_BACKUP_INTENT\n");
        if (params.CreateOptions & FILE_RESERVE_OPFILTER)
            DbgP("\tFILE_RESERVE_OPFILTER \n");
    }

    DbgP("Share Access: %s %s %s\n", 
        (params.ShareAccess & FILE_SHARE_READ)?"READ ":"",
        (params.ShareAccess & FILE_SHARE_WRITE)?"WRITE ":"",
        (params.ShareAccess & FILE_SHARE_DELETE)?"DELETE":"");

    DbgP("Desired Access: %s %s %s %s %s %s %s %s %s %s %s %s\n",
        (params.DesiredAccess & FILE_READ_DATA)?"READ":"",
        (params.DesiredAccess & STANDARD_RIGHTS_READ)?"READ_ACL":"",
        (params.DesiredAccess & FILE_READ_ATTRIBUTES)?"GETATTR":"",
        (params.DesiredAccess & FILE_READ_EA)?"READ_EA":"",
        (params.DesiredAccess & FILE_WRITE_DATA)?"WRITE":"",
        (params.DesiredAccess & STANDARD_RIGHTS_WRITE)?"WRITE_ACL":"",
        (params.DesiredAccess & FILE_WRITE_ATTRIBUTES)?"SETATTR":"",
        (params.DesiredAccess & FILE_WRITE_EA)?"WRITE_EA":"",
        (params.DesiredAccess & FILE_APPEND_DATA)?"APPEND":"",
        (params.DesiredAccess & FILE_EXECUTE)?"EXEC":"",
        (params.DesiredAccess & FILE_LIST_DIRECTORY)?"LSDIR":"",
        (params.DesiredAccess & FILE_TRAVERSE)?"TRAVERSE":"");
}

unsigned char * print_file_information_class(int InfoClass) 
{
    switch(InfoClass) {
        case FileBothDirectoryInformation:
            return (unsigned char *)"FileBothDirectoryInformation";
        case FileDirectoryInformation:
            return (unsigned char *)"FileDirectoryInformation";
        case FileFullDirectoryInformation:
            return (unsigned char *)"FileFullDirectoryInformation";
        case FileIdBothDirectoryInformation:
            return (unsigned char *)"FileIdBothDirectoryInformation";
        case FileIdFullDirectoryInformation:
            return (unsigned char *)"FileIdFullDirectoryInformation";
        case FileNamesInformation:
            return (unsigned char *)"FileNamesInformation";
        case FileObjectIdInformation:
            return (unsigned char *)"FileObjectIdInformation";
        case FileQuotaInformation:
            return (unsigned char *)"FileQuotaInformation";
        case FileReparsePointInformation:
            return (unsigned char *)"FileReparsePointInformation";
        case FileAllInformation:
            return (unsigned char *)"FileAllInformation";
        case FileAttributeTagInformation:
            return (unsigned char *)"FileAttributeTagInformation";
        case FileBasicInformation:
            return (unsigned char *)"FileBasicInformation";
        case FileCompressionInformation:
            return (unsigned char *)"FileCompressionInformation";
        case FileEaInformation:
            return (unsigned char *)"FileEaInformation";
        case FileInternalInformation:
            return (unsigned char *)"FileInternalInformation";
        case FileNameInformation:
            return (unsigned char *)"FileNameInformation";
        case FileNetworkOpenInformation:
            return (unsigned char *)"FileNetworkOpenInformation";
        case FilePositionInformation:
            return (unsigned char *)"FilePositionInformation";
        case FileStandardInformation:
            return (unsigned char *)"FileStandardInformation";
        case FileStreamInformation:
            return (unsigned char *)"FileStreamInformation";
        case FileAllocationInformation:
            return (unsigned char *)"FileAllocationInformation";
        case FileDispositionInformation:
            return (unsigned char *)"FileDispositionInformation";
        case FileEndOfFileInformation:
            return (unsigned char *)"FileEndOfFileInformation";
        case FileLinkInformation:
            return (unsigned char *)"FileLinkInformation";
        case FileRenameInformation:
            return (unsigned char *)"FileRenameInformation";
        case FileValidDataLengthInformation:
            return (unsigned char *)"FileValidDataLengthInformation";
        default:
            return (unsigned char *)"UNKNOWN";
    }
}

unsigned char *print_fs_information_class(int InfoClass)
{
    switch (InfoClass) {
        case FileFsAttributeInformation:
            return (unsigned char *)"FileFsAttributeInformation";
        case FileFsControlInformation:
            return (unsigned char *)"FileFsControlInformation";
        case FileFsDeviceInformation:
            return (unsigned char *)"FileFsDeviceInformation";
        case FileFsDriverPathInformation:
            return (unsigned char *)"FileFsDriverPathInformation";
        case FileFsFullSizeInformation:
            return (unsigned char *)"FileFsFullSizeInformation";
        case FileFsObjectIdInformation:
            return (unsigned char *)"FileFsObjectIdInformation";
        case FileFsSizeInformation:
            return (unsigned char *)"FileFsSizeInformation";
        case FileFsVolumeInformation:
            return (unsigned char *)"FileFsVolumeInformation";
        default:
            return (unsigned char *)"UNKNOWN";
    }
}

void print_caching_level(int on, ULONG flag)
{
    if (!on) return;
    switch(flag) {
        case 0: 
            DbgP("DISABLE_CACHING\n");
            break;
        case 1:
            DbgP("ENABLE_READ_CACHING\n");
            break;
        case 2:
            DbgP("ENABLE_WRITE_CACHING\n");
            break;
        case 3:
            DbgP("ENABLE_READWRITE_CACHING\n");
            break;   
    }
}

const char *opcode2string(int opcode)
{
    switch(opcode) {
    case NFS41_SHUTDOWN: return "NFS41_SHUTDOWN";
    case NFS41_MOUNT: return "NFS41_MOUNT";
    case NFS41_UNMOUNT: return "NFS41_UNMOUNT";
    case NFS41_OPEN: return "NFS41_OPEN";
    case NFS41_CLOSE: return "NFS41_CLOSE";
    case NFS41_READ: return "NFS41_READ";
    case NFS41_WRITE: return "NFS41_WRITE";
    case NFS41_LOCK: return "NFS41_LOCK";
    case NFS41_UNLOCK: return "NFS41_UNLOCK";
    case NFS41_DIR_QUERY: return "NFS41_DIR_QUERY";
    case NFS41_FILE_QUERY: return "NFS41_FILE_QUERY";
    case NFS41_FILE_SET: return "NFS41_FILE_SET";
    case NFS41_EA_SET: return "NFS41_EA_SET";
    case NFS41_SYMLINK: return "NFS41_SYMLINK";
    case NFS41_VOLUME_QUERY: return "NFS41_VOLUME_QUERY";
    default: return "UNKNOWN";
    }
}

void print_open_error(int on, int status)
{
    switch (status) {
    case ERROR_ACCESS_DENIED:
        DbgP("[ERROR] nfs41_Create: STATUS_NETWORK_ACCESS_DENIED\n");
        break;
    case ERROR_INVALID_NAME:
        DbgP("[ERROR] nfs41_Create: STATUS_OBJECT_NAME_INVALID\n");
        break;
    case ERROR_FILE_EXISTS:
        DbgP("[ERROR] nfs41_Create: ERROR_FILE_EXISTS\n");
        break;
    case ERROR_FILE_INVALID:
        DbgP("[ERROR] nfs41_Create: STATUS_FILE_INVALID\n");
        break;
    case ERROR_FILE_NOT_FOUND:
        DbgP("[ERROR] nfs41_Create: ERROR_FILE_NOT_FOUND\n");
        break;
    case ERROR_FILENAME_EXCED_RANGE:
        DbgP("[ERROR] nfs41_Create: STATUS_NAME_TOO_LONG\n");
        break;
    case ERROR_NETWORK_ACCESS_DENIED:
        DbgP("[ERROR] nfs41_Create: ERROR_NETWORK_ACCESS_DENIED\n");
        break;
    case ERROR_PATH_NOT_FOUND:
        DbgP("[ERROR] nfs41_Create: STATUS_OBJECT_PATH_NOT_FOUND\n");
        break;
    case ERROR_SHARING_VIOLATION:
        DbgP("[ERROR] nfs41_Create: STATUS_SHARING_VIOLATION\n");
        break;
    default:
        DbgP("[ERROR] nfs41_Create: upcall returned %d returning "
            "STATUS_INSUFFICIENT_RESOURCES\n", status);
    case ERROR_OUTOFMEMORY:
        DbgP("[ERROR] nfs41_Create: STATUS_INSUFFICIENT_RESOURCES\n");
    }
}

void print_wait_status(int on, const char *prefix, NTSTATUS status, 
                       const char *opcode, PVOID entry, int xid)
{
    switch (status) {
    case STATUS_SUCCESS:
        if (opcode)
            DbgP("%s Got a wakeup call, finishing %s entry=%p xid=%d\n", 
                prefix, opcode, entry, xid);
        else
            DbgP("%s Got a wakeup call\n", prefix);
        break;
    case STATUS_USER_APC:
        DbgP("%s KeWaitForSingleObject returned STATUS_USER_APC\n", prefix);
        break;
    case STATUS_ALERTED:
        DbgP("%s KeWaitForSingleObject returned STATUS_ALERTED\n", prefix);
        break;
    default:
        DbgP("%s KeWaitForSingleObject returned %d\n", prefix, status);
    }
}
/* This is taken from toaster/func.  Rumor says this should be replaced
 * with a WMI interface???
 */
ULONG
dprintk(
    IN PCHAR func,
    IN ULONG flags,
    IN PCHAR format,
    ...)
{
    #define     TEMP_BUFFER_SIZE        1024
    va_list    list;
    CHAR      debugMessageBuffer[TEMP_BUFFER_SIZE];
    NTSTATUS status, rv = STATUS_SUCCESS;

    va_start(list, format);

    if (format)
    {
        //
        // Use the safe string function, RtlStringCbVPrintfA, instead of _vsnprintf.
        // RtlStringCbVPrintfA NULL terminates the output buffer even if the message
        // is longer than the buffer. This prevents malicious code from compromising
        // the security of the system.
        //
        status = RtlStringCbVPrintfA(debugMessageBuffer, sizeof(debugMessageBuffer),
                                    format, list);

        if (!NT_SUCCESS(status))
            rv = DbgPrintEx(PNFS_FLTR_ID, DPFLTR_MASK | flags,
                            "RtlStringCbVPrintfA failed %x \n", status);
        else
            rv = DbgPrintEx(PNFS_FLTR_ID, DPFLTR_MASK | flags, "%s    %s: %s\n",
                    PNFS_TRACE_TAG, func, debugMessageBuffer);
    }
    va_end(list);

    return rv;
}

