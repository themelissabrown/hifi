//
//  FBXBaker.h
//  libraries/model-baking/src
//
//  Created by Stephen Birarda on 3/30/17.
//  Copyright 2017 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_FBXBaker_h
#define hifi_FBXBaker_h

#include <QtCore/QFutureSynchronizer>
#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkReply>

#include "Baker.h"
#include "TextureBaker.h"

#include <gpu/Texture.h> 

namespace fbxsdk {
    class FbxManager;
    class FbxProperty;
    class FbxScene;
    class FbxFileTexture;
}

static const QString BAKED_FBX_EXTENSION = ".baked.fbx";
using FBXSDKManagerUniquePointer = std::unique_ptr<fbxsdk::FbxManager, std::function<void (fbxsdk::FbxManager *)>>;

class FBXBaker : public Baker {
    Q_OBJECT
public:
    FBXBaker(const QUrl& fbxURL, const QString& baseOutputPath, bool copyOriginals = true);

    // all calls to bake must be from the same thread, because the Autodesk SDK will cause
    // a crash if it is called from multiple threads
    Q_INVOKABLE virtual void bake() override;

    QUrl getFBXUrl() const { return _fbxURL; }
    QString getBakedFBXRelativePath() const { return _bakedFBXRelativePath; }

signals:
    void allTexturesBaked();

    void sourceCopyReadyToLoad();

private slots:
    void bakeSourceCopy();
    void handleFBXNetworkReply();
    void handleBakedTexture();

private:
    void setupOutputFolder();

    void loadSourceFBX();

    void bakeCopiedFBX();

    void importScene();
    void rewriteAndBakeSceneTextures();
    void exportScene();
    void removeEmbeddedMediaFolder();
    void possiblyCleanupOriginals();

    void checkIfTexturesFinished();

    QString createBakedTextureFileName(const QFileInfo& textureFileInfo);
    QUrl getTextureURL(const QFileInfo& textureFileInfo, fbxsdk::FbxFileTexture* fileTexture);

    void bakeTexture(const QUrl& textureURL, gpu::TextureType textureType, const QString& destinationFilePath);

    QString pathToCopyOfOriginal() const;

    QUrl _fbxURL;
    QString _fbxName;
    
    QString _baseOutputPath;
    QString _uniqueOutputPath;
    QString _bakedFBXRelativePath;

    static FBXSDKManagerUniquePointer _sdkManager;
    fbxsdk::FbxScene* _scene { nullptr };

    QHash<QUrl, QString> _unbakedTextures;
    QHash<QString, int> _textureNameMatchCount;

    QSet<QSharedPointer<TextureBaker>> _bakingTextures;
    QFutureSynchronizer<void> _textureBakeSynchronizer;

    bool _copyOriginals { true };

    bool _pendingErrorEmission { false };
};

#endif // hifi_FBXBaker_h
