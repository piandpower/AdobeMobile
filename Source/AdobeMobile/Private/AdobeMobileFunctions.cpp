//
//  Created by Robert Segal on 2016-03-31.
//  Copyright (c) 2016 Get Set Games Inc. All rights reserved.
//

#include "AdobeMobileFunctions.h"
#include "AdobeMobilePrivatePCH.h"

#if PLATFORM_IOS
#import "../../lib/iOS/ADBMobile.h"
#elif PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "AndroidApplication.h"
#endif

#if PLATFORM_ANDROID
void AdobeMobile_CreateJavaKeyValueArrays(JNIEnv *Env, jobjectArray &jKeysArray, jobjectArray &jValuesArray, TArray<FString> keys, TArray<FString> values)
{
    for (uint32 Param = 0; Param < keys.Num(); Param++)
    {
        jstring StringValue = Env->NewStringUTF(TCHAR_TO_UTF8(*keys[Param]));
        
        Env->SetObjectArrayElement(jKeysArray, Param, StringValue);
        Env->DeleteLocalRef(StringValue);
        
        StringValue = Env->NewStringUTF(TCHAR_TO_UTF8(*values[Param]));
        
        Env->SetObjectArrayElement(jValuesArray, Param, StringValue);
        Env->DeleteLocalRef(StringValue);
    }
}
#endif

void UAdobeMobileFunctions::AdobeMobileTrackState(FString state, TArray<FString> dataKeys, TArray<FString> dataValues)
{
    UE_LOG(LogAdobeMobile, Log, TEXT("Tracking State..."));

    const int32 kNumKeys   = dataKeys.Num();
    const int32 kNumValues = dataValues.Num();
    
    if (kNumKeys == 0 || kNumValues == 0)
    {
        UE_LOG(LogAdobeMobile, Log, TEXT("keys and/or value arguments are empty"));
        return;
    }
    
    if (kNumKeys != kNumValues)
    {
         UE_LOG(LogAdobeMobile, Log, TEXT("number of keys are not equal to number of values"));
        return;
    }
    
    // Verify that keys are of allowed data types by Adobe Mobile plugin
    
#if PLATFORM_IOS
    NSMutableDictionary *d = [NSMutableDictionary dictionaryWithCapacity:kNumKeys];
    
    for (uint32 i = 0; i < kNumKeys; i++)
    {
        FString &k = dataKeys[i];
        FString &v = dataValues[i];
        
        d[k.GetNSString()] = v.GetNSString();
    }
    
    [ADBMobile trackState:state.GetNSString() data:d];
    
#elif PLATFORM_ANDROID
    if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
    {
        static jmethodID Method = FJavaWrapper::FindMethod(Env,
                                                           FJavaWrapper::GameActivityClassID,
                                                           "AndroidThunkJava_AdobeMobile_TrackState",
                                                           "(Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/String;)V",
                                                           false);
        
        jobjectArray jKeys   = (jobjectArray)Env->NewObjectArray(dataKeys.Num(),   FJavaWrapper::JavaStringClass, NULL);
        jobjectArray jValues = (jobjectArray)Env->NewObjectArray(dataValues.Num(), FJavaWrapper::JavaStringClass, NULL);
        
        AdobeMobile_CreateJavaKeyValueArrays(Env, jKeys, jValues, dataKeys, dataValues);
        
        jstring jState = Env->NewStringUTF(TCHAR_TO_UTF8(*state));
        
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method, jState, jKeys, jValues);
        
        Env->DeleteLocalRef(jKeys);
        Env->DeleteLocalRef(jValues);
        Env->DeleteLocalRef(jState);
    }
#endif
}

void UAdobeMobileFunctions::AdobeMobileTrackAction(FString state, TArray<FString> dataKeys, TArray<FString> dataValues)
{
    UE_LOG(LogAdobeMobile, Log, TEXT("Tracking Action..."));
    
    const int32 kNumKeys   = dataKeys.Num();
    const int32 kNumValues = dataValues.Num();
    
    if (kNumKeys == 0 || kNumValues == 0)
    {
        UE_LOG(LogAdobeMobile, Log, TEXT("keys and/or value arguments are empty"));
        return;
    }
    
    if (kNumKeys != kNumValues)
    {
        UE_LOG(LogAdobeMobile, Log, TEXT("number of keys are not equal to number of values"));
        return;
    }
    
#if PLATFORM_IOS
    NSMutableDictionary *d = [NSMutableDictionary dictionaryWithCapacity:kNumKeys];
    
    for (uint32 i = 0; i < kNumKeys; i++)
    {
        FString &k = dataKeys[i];
        FString &v = dataValues[i];
        
        d[k.GetNSString()] = v.GetNSString();
    }
    
    [ADBMobile trackAction:state.GetNSString() data:d];
#elif PLATFORM_ANDROID
    if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
    {
        static jmethodID Method = FJavaWrapper::FindMethod(Env,
                                                           FJavaWrapper::GameActivityClassID,
                                                           "AndroidThunkJava_AdobeMobile_TrackAction",
                                                           "(Ljava/lang/String;[Ljava/lang/String;[Ljava/lang/String;)V",
                                                           false);
        
        jobjectArray jKeys   = (jobjectArray)Env->NewObjectArray(dataKeys.Num(),   FJavaWrapper::JavaStringClass, NULL);
        jobjectArray jValues = (jobjectArray)Env->NewObjectArray(dataValues.Num(), FJavaWrapper::JavaStringClass, NULL);
        
        AdobeMobile_CreateJavaKeyValueArrays(Env, jKeys, jValues, dataKeys, dataValues);

        jstring jState = Env->NewStringUTF(TCHAR_TO_UTF8(*state));
        
        FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method, jState, jKeys, jValues);
        
        Env->DeleteLocalRef(jKeys);
        Env->DeleteLocalRef(jValues);
        Env->DeleteLocalRef(jState);
    }
#endif
}


