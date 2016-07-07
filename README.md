# 4分木空間分割衝突判定モジュール For cocos2d-x v3

## Description
cocos2d-xのv3用の4分木空間分割の衝突判定モジュールクラスです。
cocos2d::Nodeの衝突判定をある程度効率的に行うことができるかも。

以下のGitHubコードを参考にしています。  
<https://github.com/gear1554/CollisionDetectionTest>

ブログと性能調査  
<http://brbranch.jp/blog/201607/cocos2d-x/fourtreecollision>

## 動作確認環境

* cocos2d-x 3.10
※たぶん3系なら動きます。

## 使い方
詳しいサンプルはClasses/SampleLayer.cpp内のコードを参照下さい。

1. Classes/collision内のフォルダをプロジェクトに入れる。
2. ヘッダーへの定義を行う
3. CollisionDetactionクラスの取得

```cpp
 // シングルトンを利用する場合
 auto detaction = CollisionDetection::getDefaultDetaction();
 // 新たに作成する場合
 auto detaction = CollisionDetaction::create();
```

4. CollisionDetactionクラスの初期化

```cpp
 // cocos2d::Node* fieldNode : 衝突判定が行われるフィールド
 // int level : 4分木衝突判定の階層
 // bool autoclean : fieldNodeがremoveされた際に自動で後処理をするかどうか
 // std::function<void(CollisionNode* , CollisionNode*)> func : 衝突判定処理
 // 注意：この関数を呼び出した後にfieldNode##setonExitTransitionDidStartCallbackを設定した場合
 //       自動で後処理されなくなります。
 detaction->init(fieldNode , level , autoclean , [this](CollisionNode* node1, CollisionNode* node2){
  // 衝突判定処理
 });
```

5. 衝突判定オブジェクトの登録
* CollisionComponentをNodeに追加することで自動でCollisionDetactionクラスに登録／除去が行われます。

```cpp
 auto object = Sprite::create("image.png");
 /* 
   衝突判定自動登録／削除コンポーネント
   addChild/removeChildされたタイミングで
   自動的に衝突判定モジュールに追加／除去されます
   数字の「1」は、それがどのグループに属するかどうか(敵とか味方とか弾とか)
 */
 // シングルトンのCollisionDetactionクラスを利用する場合
 auto component = CollisionComponent::create(1);
 // 衝突判定モジュールを指定する場合
 auto component = CollisionComponent::create(detaction, 1);

 // 衝突判定に指定
 object->addComponent(component);
 fieldNode->addChild(object);
```

* CollisionComponentを利用せずに手動で登録／除去を行う場合

```cpp
 auto collisionNode = CollisionNode::create(object , 1);
 // 追加
 fieldNode->addChld(object);
 detaction->add(collisionNode);
 
 // 除去
 fieldNode->removeChild(object);
 detaction->remove(collisionNode);
 // 除去はNode指定でもできます
 detaction->remove(object);
```

6. 毎フレーム処理を行う箇所でupdate()を呼び出す

```cpp
 void CollisionSampleLayer::update(float f) {
     detaction->update();
 }
```

* 衝突判定処理の例

```cpp
 [](CollisionNode* node1 , CollisionNode* node2) {
     // グループが同じなら何も処理しない
     if(node1->isSameGroup(node2)){
         return;
     }
     // 衝突していなければ何もしない
     // 以下は回転もある程度考慮した衝突判定を行うメソッドです。(CollisionUtils.hpp)
     if(!CollisionUtils::intersectRect(node1, node2)){
         return;
     }
     // ビットマスクで評価
     // グループが1、2、4のどれかなら真
     if(node1->bitmask(1 | 2 | 4)) {
         // node1への処理
     }
     // 指定したグループなら
     if(node2->is(1)){
         // node2への評価
     }
 }
```
## デモ動画
<https://www.youtube.com/watch?v=6ZHY832taZU>

## License
This software is released under the MIT License, see LICENSE.txt.
