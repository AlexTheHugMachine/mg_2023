#include "qte.h"
#include "implicits.h"
#include "ui_interface.h"
#include "node.h"
#include "sdf.h"
#include <iostream>

using namespace std;

int RX, RY, RZ;
ID_Mesh id_m;

MainWindow::MainWindow() : QMainWindow(), uiw(new Ui::Assets)
{
    // Chargement de l'interface
    uiw->setupUi(this);

    // Chargement du GLWidget
    meshWidget = new MeshWidget;
    QGridLayout* GLlayout = new QGridLayout;
    GLlayout->addWidget(meshWidget, 0, 0);
    GLlayout->setContentsMargins(0, 0, 0, 0);
    uiw->widget_GL->setLayout(GLlayout);

    // Creation des connect
    CreateActions();

    meshWidget->SetCamera(Camera(Vector(10, 0, 0), Vector(0.0, 0.0, 0.0)));
}

MainWindow::~MainWindow()
{
    delete meshWidget;
}

void MainWindow::CreateActions()
{
    // Buttons
    connect(uiw->boxMesh, SIGNAL(clicked()), this, SLOT(BoxMeshExample()));
    connect(uiw->sphere, &QPushButton::clicked, [=]() {
        spawnSphere();
        id_m = ID_SPHERE;
    });
    connect(uiw->cube, &QPushButton::clicked, [=]() {
        spawnCube();
        id_m = ID_CUBE;
    });
    connect(uiw->cone, &QPushButton::clicked, [=]() {
        spawnCone();
        id_m = ID_CONE;
    });
    connect(uiw->tore, &QPushButton::clicked, [=]() {
        spawnTore();
        id_m = ID_TORE;
    });
    connect(uiw->capsule, &QPushButton::clicked, [=]() {
        spawnCapsule();
        id_m = ID_CAPSULE;
    });

    connect(uiw->translation, &QPushButton::clicked, [=]() {
        OperationExample(TRANSLATION);
    });
    connect(uiw->rotation, &QPushButton::clicked, [=]() {
        OperationExample(ROTATION);
    });
    connect(uiw->scale, &QPushButton::clicked, [=]() {
        OperationExample(SCALE);
    });

    connect(uiw->union_, &QPushButton::clicked, [=]() {
        OperationExample(UNION);
    });
    connect(uiw->intersection, &QPushButton::clicked, [=]() {
        OperationExample(INTERSECTION);
    });
    connect(uiw->difference, &QPushButton::clicked, [=]() {
        OperationExample(DIFF);
    });
    connect(uiw->union_lisse, &QPushButton::clicked, [=]() {
        OperationExample(UNION_SMOOTH);
    });
    connect(uiw->intersection_lisse, &QPushButton::clicked, [=]() {
        OperationExample(INTERSECTION_SMOOTH);
    });
    connect(uiw->difference_lisse, &QPushButton::clicked, [=]() {
        OperationExample(DIFF_SMOOTH);
    });

    connect(uiw->composition, SIGNAL(clicked()), this, SLOT(originalPrimitivesComposition()));
    connect(uiw->resetcameraButton, SIGNAL(clicked()), this, SLOT(ResetCamera()));
    connect(uiw->wireframe, SIGNAL(clicked()), this, SLOT(UpdateMaterial()));
    connect(uiw->radioShadingButton_1, SIGNAL(clicked()), this, SLOT(UpdateMaterial()));
    connect(uiw->radioShadingButton_2, SIGNAL(clicked()), this, SLOT(UpdateMaterial()));
    connect(uiw->RotateXslider, SIGNAL(valueChanged(int)), this, SLOT(meshRotation()));
    connect(uiw->RotateYslider, SIGNAL(valueChanged(int)), this, SLOT(meshRotation()));
    connect(uiw->RotateZslider, SIGNAL(valueChanged(int)), this, SLOT(meshRotation()));

    // Widget edition
    connect(meshWidget, SIGNAL(_signalEditSceneLeft(const Ray&)), this, SLOT(editingSceneLeft(const Ray&)));
    connect(meshWidget, SIGNAL(_signalEditSceneRight(const Ray&)), this, SLOT(editingSceneRight(const Ray&)));
}

void MainWindow::editingSceneLeft(const Ray&)
{
}

void MainWindow::editingSceneRight(const Ray&)
{
}

void MainWindow::BoxMeshExample()
{
    Mesh boxMesh = Mesh(Box(1.0));

    std::vector<Color> cols;
    cols.resize(boxMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(double(i) / 6.0, fmod(double(i) * 39.478378, 1.0), 0.0);

    meshColor = MeshColor(boxMesh, cols, boxMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::spawnCube()
{
    Mesh implicitMesh;
    Cube cube = Cube(Vector(0, 0, 0), Vector(3, 3, 3));
    SDF sdf = SDF(&cube);

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::spawnTore()
{
    Mesh implicitMesh;
    Tore tore = Tore(1, 3);
    SDF sdf = SDF(&tore);

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::spawnSphere()
{
    Mesh implicitMesh;
    Sphere sphere = Sphere(Vector(0, 0, 0), 2);
    SDF sdf = SDF(&sphere);

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::spawnCone()
{
    Mesh implicitMesh;
    Cone cone = Cone(0.2, 0.6, 5);
    SDF sdf = SDF(&cone);

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::spawnCapsule()
{
    Mesh implicitMesh;
    Capsule capsule = Capsule(Vector(0, -3, 0), Vector(0, 3, 0), 2);
    SDF sdf = SDF(&capsule);

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::on_RotateXslider_valueChanged(int value)
{
    RX = value;
}

void MainWindow::on_RotateYslider_valueChanged(int value)
{
    RY = value;
}

void MainWindow::on_RotateZslider_valueChanged(int value)
{
    RZ = value;
}

void MainWindow::meshRotation()
{
    Mesh implicitMesh;
    Rotation * rotationPrimitives;
    SDF sdf;
    Sphere * sphere_rotationX;
    Cube * cube_rotationX;
    Cone * cone_rotationX;
    Tore * tore_rotationX;
    Capsule * capsule_rotationX;

    switch (id_m)
    {
    case ID_SPHERE:
        sphere_rotationX = new Sphere(Vector(0, 0, 0), 2);
        rotationPrimitives = new Rotation(sphere_rotationX, Vector(RX, RY, RZ));
        sdf = SDF(rotationPrimitives);
        break;
    case ID_CUBE:
        cube_rotationX = new Cube(Vector(0, 0, 0), Vector(3, 3, 3));
        rotationPrimitives = new Rotation(cube_rotationX, Vector(RX, RY, RZ));
        sdf = SDF(rotationPrimitives);
        break;
    case ID_CONE:
        cone_rotationX = new Cone(0.2, 0.6, 5);
        rotationPrimitives = new Rotation(cone_rotationX, Vector(RX, RY, RZ));
        sdf = SDF(rotationPrimitives);
        break;
    case ID_TORE:
        tore_rotationX = new Tore(1, 3);
        rotationPrimitives = new Rotation(tore_rotationX, Vector(RX, RY, RZ));
        sdf = SDF(rotationPrimitives);
        break;
    case ID_CAPSULE:
        capsule_rotationX = new Capsule(Vector(0, -3, 0), Vector(0, 3, 0), 2);
        rotationPrimitives = new Rotation(capsule_rotationX, Vector(RX, RY, RZ));
        sdf = SDF(rotationPrimitives);
        break;
    }

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}


void MainWindow::OperationExample(Operation operation)
{
    Mesh implicitMesh;
    SDF sdf;
    Sphere sphere;
    Cube cube;
    Cone cone;
    Tore tore;
    Capsule capsule;

    Sphere * sphere_operator1;
    Sphere* sphere_operator2;
    Tore * tore_operator1;

    Translation * translationPrimitives;
    Rotation * rotationPrimitives;
    Scale * scalePrimitives;

    Union * unionPrimitives;
    Intersection * intersectionPrimitives;
    Difference * differencePrimitives;

    UnionSmooth * unionPrimitivesSmooth;
    IntersectionSmooth * intersectionPrimitivesSmooth;
    DifferenceSmooth * differencePrimitivesSmooth;


    switch (operation)
    {

    /* PRIMITIVES */
    case SPHERE:
        sphere = Sphere(Vector(0, 0, 0), 2);
        sdf = SDF(&sphere);
        break;
    case CUBE:
        cube = Cube(Vector(0, 0, 0), Vector(3, 3, 3));
        sdf = SDF(&cube);
        break;
    case CONE:
        cone = Cone(0.2, 0.6, 5);
        sdf = SDF(&cone);
        break;
    case TORE:
        tore = Tore(1, 3);
        sdf = SDF(&tore);
        break;
    case CAPSULE:
        capsule = Capsule(Vector(0, -3, 0), Vector(0, 3, 0), 2);
        sdf = SDF(&capsule);
        break;

    /* UNARY OPERATORS */
    case TRANSLATION:
        sphere_operator1 = new Sphere(Vector(0, 0, 0), 2);
        translationPrimitives = new Translation(sphere_operator1, Vector(-1, -2, 1));
        sdf = SDF(translationPrimitives);
        break;
    case ROTATION:
        tore_operator1 = new Tore(1, 3);
        rotationPrimitives = new Rotation(tore_operator1, Vector(0, 0, 60));
        sdf = SDF(rotationPrimitives);
        break;
    case SCALE:
        sphere_operator1 = new Sphere(Vector(0, 0, 0), 2);
        scalePrimitives = new Scale(sphere_operator1, Vector(0.4, 0.4, 0.4));
        sdf = SDF(scalePrimitives);
        break;


    /* BINARY OPERATORS */
    case UNION:
        sphere_operator1 = new Sphere(Vector(-1, 0, 0), 2);
        sphere_operator2 = new Sphere(Vector(1, 0, 0), 2);
        unionPrimitives = new Union(sphere_operator1, sphere_operator2);
        sdf = SDF(unionPrimitives);
        break;
    case INTERSECTION:
        sphere_operator1 = new Sphere(Vector(-1, 0, 0), 2);
        sphere_operator2 = new Sphere(Vector(1, 0, 0), 2);
        intersectionPrimitives = new Intersection(sphere_operator1, sphere_operator2);
        sdf = SDF(intersectionPrimitives);
        break;
    case DIFF:
        sphere_operator1 = new Sphere(Vector(-1, 0, 0), 2);
        sphere_operator2 = new Sphere(Vector(1, 0, 0), 2);
        differencePrimitives = new Difference(sphere_operator1, sphere_operator2);
        sdf = SDF(differencePrimitives);
        break;
    case UNION_SMOOTH:
        sphere_operator1 = new Sphere(Vector(-1, 0, 0), 2);
        sphere_operator2 = new Sphere(Vector(1, 0, 0), 2);
        unionPrimitivesSmooth = new UnionSmooth(sphere_operator1, sphere_operator2, 4);
        sdf = SDF(unionPrimitivesSmooth);
        break;
    case INTERSECTION_SMOOTH:
        sphere_operator1 = new Sphere(Vector(-1, 0, 0), 2);
        sphere_operator2 = new Sphere(Vector(1, 0, 0), 2);
        intersectionPrimitivesSmooth = new IntersectionSmooth(sphere_operator1, sphere_operator2, 4);
        sdf = SDF(intersectionPrimitivesSmooth);
        break;
    case DIFF_SMOOTH:
        sphere_operator1 = new Sphere(Vector(-1, 0, 0), 2);
        sphere_operator2 = new Sphere(Vector(1, 0, 0), 2);
        differencePrimitivesSmooth = new DifferenceSmooth(sphere_operator1, sphere_operator2, 4);
        sdf = SDF(differencePrimitivesSmooth);
        break;

    default:
        cout << "Veuillez entrer un type de primitive valide!" << endl;
        break;
    }

    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();

}

void MainWindow::originalPrimitivesComposition()
{
    Mesh implicitMesh;

    const double smoothFactor = 2;

    Tore tore1(0.5, 2);
    Tore tore2(0.5, 2);
    Tore tore3(0.5, 2);
    Tore tore4(0.5, 2);
    Tore tore5(0.5, 2);
    Tore tore6(0.5, 2);

    Rotation * rotation1 = new Rotation(&tore2, Vector(0, 0, 90));
    Translation * translation1 = new Translation(rotation1, Vector(2, 2, 0));
    UnionSmooth * unionSmooth1 = new UnionSmooth(&tore1, translation1, smoothFactor);

    Rotation* rotation2 = new Rotation(&tore3, Vector(0, 0, 90));
    Translation* translation2 = new Translation(rotation2, Vector(-2, 2, 0));
    UnionSmooth* unionSmooth2 = new UnionSmooth(unionSmooth1, translation2, smoothFactor);

    Translation* translation3 = new Translation(&tore4, Vector(0, 4, 0));
    UnionSmooth* unionSmooth3 = new UnionSmooth(unionSmooth2, translation3, smoothFactor);

    Rotation* rotation3 = new Rotation(&tore5, Vector(0, 90, 90));
    Translation* translation4 = new Translation(rotation3, Vector(0, 2, 2));
    UnionSmooth* unionSmooth4 = new UnionSmooth(unionSmooth3, translation4, smoothFactor);

    Rotation* rotation4 = new Rotation(&tore6, Vector(0, 90, 90));
    Translation* translation5 = new Translation(rotation4, Vector(0, 2, -2));
    UnionSmooth* unionSmooth5 = new UnionSmooth(unionSmooth4, translation5, smoothFactor);

    SDF sdf(unionSmooth5);
    sdf.Polygonize(50, implicitMesh, Box(5.0));

    std::vector<Color> cols;
    cols.resize(implicitMesh.Vertexes());
    for (size_t i = 0; i < cols.size(); i++)
        cols[i] = Color(0.8, 0.8, 0.8);

    meshColor = MeshColor(implicitMesh, cols, implicitMesh.VertexIndexes());
    UpdateGeometry();
}

void MainWindow::UpdateGeometry()
{
    meshWidget->ClearAll();
    meshWidget->AddMesh("BoxMesh", meshColor);

    uiw->lineEdit->setText(QString::number(meshColor.Vertexes()));
    uiw->lineEdit_2->setText(QString::number(meshColor.Triangles()));

    UpdateMaterial();
}

void MainWindow::UpdateMaterial()
{
    meshWidget->UseWireframeGlobal(uiw->wireframe->isChecked());

    if (uiw->radioShadingButton_1->isChecked())
        meshWidget->SetMaterialGlobal(MeshMaterial::Normal);
    else
        meshWidget->SetMaterialGlobal(MeshMaterial::Color);
}

void MainWindow::ResetCamera()
{
    meshWidget->SetCamera(Camera(Vector(-10.0), Vector(0.0)));
}
