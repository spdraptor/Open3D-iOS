// swift-tools-version:5.3

import PackageDescription

let package = Package(
    name: "Open3D-iOS",
    products: [
        .library(
            name: "Open3D-iOS",
            targets: [
                "LinkOpen3D",
                "Open3DSupport",
                "Assimp",
                "Faiss",
                "IrrXML",
                "JPEG",
                "jsoncpp",
                "libOpen3D_3rdparty_liblzf",
                "libOpen3D_3rdparty_qhull_r",
                "libOpen3D_3rdparty_qhullcpp",
                "libOpen3D_3rdparty_rply",
                "libOpen3D",
                "png",
                "pybind",
                "TBB",
            ]),
    ],
    dependencies: [
        .package(url: "https://github.com/kewlbear/NumPy-iOS.git", .branch("main")),
        .package(url: "https://github.com/kewlbear/LAPACKE-iOS.git", .branch("main")),
        .package(url: "https://github.com/kewlbear/BLAS-LAPACK-AppStore-Workaround.git", .branch("main")),
    ],
    targets: [
        .binaryTarget(name: "Assimp", url: "https://github.com/kewlbear/Open3D-iOS/releases/download/0.0.20220804014308/Assimp.xcframework.zip", checksum: "6904cc87d6001158a44caffb854e32b30e29adb606f46472d5f8ce00b9721a24"),
        .binaryTarget(name: "Faiss", url: "https://github.com/kewlbear/Open3D-iOS/releases/download/0.0.20220804014308/Faiss.xcframework.zip", checksum: "1d223aee5b8c550f67e937da850e9a612fbfd358b2b9573b1c9e377f29e59a72"),
        .binaryTarget(name: "IrrXML", url: "https://github.com/kewlbear/Open3D-iOS/releases/download/0.0.20220804014308/IrrXML.xcframework.zip", checksum: "fd60a0ad4f9330f7a6c9553621e0ba4cba1d678a89bbb2b5bfce730f8b39235b"),
        .binaryTarget(name: "JPEG", url: "https://github.com/kewlbear/Open3D-iOS/releases/download/0.0.20220804014308/JPEG.xcframework.zip", checksum: "1a9cd104c1600703a52d6917a7c2213417f6c456ccef06688580b9f6b8bd4503"),
        .binaryTarget(name: "jsoncpp", url: "https://github.com/kewlbear/Open3D-iOS/releases/download/0.0.20220804014308/jsoncpp.xcframework.zip", checksum: "350af41078a47080385fd60e5ecc2f33c2cb21a91557cb6b5233f38b306f50a4"),
        .binaryTarget(name: "libOpen3D_3rdparty_liblzf", path: "artifacts/libOpen3D_3rdparty_liblzf.xcframework"),
        .binaryTarget(name: "libOpen3D_3rdparty_qhull_r", path: "artifacts/libOpen3D_3rdparty_qhull_r.xcframework"),
        .binaryTarget(name: "libOpen3D_3rdparty_qhullcpp", path: "artifacts/libOpen3D_3rdparty_qhullcpp.xcframework"),
        .binaryTarget(name: "libOpen3D_3rdparty_rply", path: "artifacts/libOpen3D_3rdparty_rply.xcframework"),
        .binaryTarget(name: "libOpen3D", path: "artifacts/libOpen3D.xcframework"),
        .binaryTarget(name: "png", url: "https://github.com/kewlbear/Open3D-iOS/releases/download/0.0.20220804014308/png.xcframework.zip", checksum: "0fd051316b200e5663d6d92e2efb24d0c012b417119733be9a498559cae173ea"),
        .binaryTarget(name: "pybind", path: "artifacts/pybind.xcframework"),
        .binaryTarget(name: "TBB", url: "https://github.com/kewlbear/Open3D-iOS/releases/download/0.0.20220804014308/TBB.xcframework.zip", checksum: "9c5d9a4aa45ca15cec514a36faf10709a44af8e780441bf9c6d82a0fd0c64e60"),
        .target(
            name: "LinkOpen3D",
            dependencies: [
                "NumPy-iOS",
                "LAPACKE-iOS",
                "Assimp",
                "Faiss",
                "IrrXML",
                "JPEG",
                "jsoncpp",
                "libOpen3D_3rdparty_liblzf",
                "libOpen3D_3rdparty_qhull_r",
                "libOpen3D_3rdparty_qhullcpp",
                "libOpen3D_3rdparty_rply",
                "libOpen3D",
                "png",
                "pybind",
                "TBB",
                "BLAS-LAPACK-AppStore-Workaround",
            ],
            linkerSettings: [
                .linkedLibrary("stdc++"),
            ]
        ),
        .target(
            name: "Open3DSupport",
            resources: [.copy("site-packages")]),
        .testTarget(
            name: "Open3D-iOSTests",
            dependencies: ["Open3DSupport"]),
    ]
)
